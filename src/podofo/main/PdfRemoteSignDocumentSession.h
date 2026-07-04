// PdfRemoteSignDocumentSession.h
/**
 * @file PdfRemoteSignDocumentSession.h
 * @brief High-level API for remote PDF signing and LTA timestamping using PoDoFo and OpenSSL.
 *
 * This header declares the `PdfRemoteSignDocumentSession` which orchestrates a two-phase
 * remote signing flow (hash extraction and signature injection), helpers for DSS/LTV
 * (embedded validation material), and a `PdfDocTimeStampSigner` for RFC3161 DocTimeStamp.
 *
 * Ported from eu-digital-identity-wallet/eudi-lib-podofo (a fork of an older PoDoFo) to
 * this fork's newer signing API. Notable adaptations from the original:
 *  - The end-entity certificate and chain certificates are no longer passed together to
 *    a 3-arg PdfSignerCms constructor (that overload doesn't exist here). Instead the
 *    chain is added post-construction via PdfSignerCms::AddCertificate(), a small
 *    additive method on PdfSignerCms/CmsContext that embeds extra X.509 certificates in
 *    the CMS "certificates" collection.
 *  - PdfSignerCms has no SetTimestampToken() method here. The RFC3161 signature
 *    timestamp is instead attached as a CMS unsigned attribute (id-aa-signatureTimeStampToken,
 *    OID 1.2.840.113549.1.9.16.2.14) via the existing PdfSignerCms::AddAttribute().
 */
#ifndef PDF_DSS_SIGNING_SESSION_H
#define PDF_DSS_SIGNING_SESSION_H

#ifdef _MSC_VER
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <regex>
#include <thread>
#include <filesystem>
#include <map>
#include <utility>

#include <podofo/podofo.h>

// Deliberately no <openssl/...> includes here: this is a PUBLIC header
// (installed alongside the rest of podofo/main), and OpenSSL is linked
// PRIVATE to the podofo library target (see root CMakeLists.txt) — same
// reason PdfSignerCms.h/CmsContext.h forward-declare OpenSSL types instead
// of including their headers. Any OpenSSL type actually needed here would
// leak that private dependency into every consumer of <podofo/podofo.h>
// (this broke the plain library-consumer builds: examples/tools don't get
// OpenSSL's include path, only the podofo library target itself does).
// The one OpenSSL type this header used to reference (BIO, via BioFreeAll/
// BioPtr) was pure implementation detail with no public API surface, so it
// moved into PdfRemoteSignDocumentSession.cpp instead of being forward-declared.

namespace fs = std::filesystem;

namespace PoDoFo {

    /**
     * @brief Container for validation-related artifacts to embed into the PDF DSS.
     *
     * Holds base64-encoded DER blobs for certificates, CRLs and OCSP responses.
     */
    class ValidationData {
    public:
        /** Default constructor */
        ValidationData() = default;

        /**
         * @brief Construct with initial collections.
         * @param certificates Base64 DER-encoded certificates to embed.
         * @param crls Base64 DER-encoded CRLs to embed.
         * @param ocsps Base64 DER-encoded OCSP responses to embed.
         */
        ValidationData(const std::vector<std::string>& certificates,
            const std::vector<std::string>& crls = {},
            const std::vector<std::string>& ocsps = {})
            : certificatesBase64(certificates), crlsBase64(crls), ocspsBase64(ocsps) {
        }

        /**
         * @brief Adds a single certificate to the validation data
         * @param certBase64 Base64-encoded certificate data
         */
        void addCertificate(const std::string& certBase64) {
            certificatesBase64.push_back(certBase64);
        }

        /**
         * @brief Adds a single CRL to the validation data
         * @param crlBase64 Base64-encoded CRL data
         */
        void addCRL(const std::string& crlBase64) {
            crlsBase64.push_back(crlBase64);
        }

        /**
         * @brief Adds a single OCSP response to the validation data
         * @param ocspBase64 Base64-encoded OCSP response data
         */
        void addOCSP(const std::string& ocspBase64) {
            ocspsBase64.push_back(ocspBase64);
        }

        /**
         * @brief Adds multiple certificates to the validation data
         * @param certs Vector of base64-encoded certificate data
         */
        void addCertificates(const std::vector<std::string>& certs) {
            certificatesBase64.insert(certificatesBase64.end(), certs.begin(), certs.end());
        }

        /**
         * @brief Adds multiple CRLs to the validation data
         * @param crls Vector of base64-encoded CRL data
         */
        void addCRLs(const std::vector<std::string>& crls) {
            crlsBase64.insert(crlsBase64.end(), crls.begin(), crls.end());
        }

        /**
         * @brief Adds multiple OCSP responses to the validation data
         * @param ocsps Vector of base64-encoded OCSP response data
         */
        void addOCSPs(const std::vector<std::string>& ocsps) {
            ocspsBase64.insert(ocspsBase64.end(), ocsps.begin(), ocsps.end());
        }

        /** Clear all stored artifacts. */
        void clear() {
            certificatesBase64.clear();
            crlsBase64.clear();
            ocspsBase64.clear();
        }

        /**
         * @return true if no artifacts are present.
         */
        bool empty() const {
            return certificatesBase64.empty() && crlsBase64.empty() && ocspsBase64.empty();
        }

        /** @return Number of certificates. */
        size_t certificateCount() const { return certificatesBase64.size(); }
        /** @return Number of CRLs. */
        size_t crlCount() const { return crlsBase64.size(); }
        /** @return Number of OCSP responses. */
        size_t ocspCount() const { return ocspsBase64.size(); }

        // Public access to vectors (for backward compatibility)
        std::vector<std::string> certificatesBase64;
        std::vector<std::string> crlsBase64;
        std::vector<std::string> ocspsBase64;
    };

    /**
     * @brief Supported message digest algorithms for CMS/TSP.
     */
    enum class HashAlgorithm {
        SHA256,  /**< SHA-256 (OID 2.16.840.1.101.3.4.2.1) */
        SHA384,  /**< SHA-384 (OID 2.16.840.1.101.3.4.2.2) */
        SHA512,  /**< SHA-512 (OID 2.16.840.1.101.3.4.2.3) */
        Unknown  /**< Not recognized */
    };

    /**
     * @brief Represents a single PDF remote signing session.
     *
     * The flow is split into two steps:
     * 1) beginSigning(): prepares the PDF and returns a base64-encoded hash to be signed remotely.
     * 2) finishSigning(): injects the signed value and optionally adds DSS/LTV material.
     *
     * Additional helpers support LTA DocTimeStamp creation and validation data embedding.
     */
    class PODOFO_API PdfRemoteSignDocumentSession final {
    public:
        /**
         * @brief Construct a signing session with full configuration.
         * @param conformanceLevel One of ADES_B_B, ADES_B_T, ADES_B_LT, ADES_B_LTA.
         * @param hashAlgorithmOid Digest OID string (e.g. 2.16.840.1.101.3.4.2.1 for SHA-256).
         * @param documentInputPath Source PDF path.
         * @param documentOutputPath Destination PDF path.
         * @param endCertificateBase64 End-entity certificate, base64 DER.
         * @param certificateChainBase64 Certificate chain, each item base64 DER.
         * @param rootEntityCertificateBase64 Optional root certificate, base64 DER.
         * @param label Optional label for diagnostics.
         */
        PdfRemoteSignDocumentSession(
            const std::string& conformanceLevel,
            const std::string& hashAlgorithmOid,
            const std::string& documentInputPath,
            const std::string& documentOutputPath,
            const std::string& endCertificateBase64,
            const std::vector<std::string>& certificateChainBase64,
            const std::optional<std::string>& rootEntityCertificateBase64 = std::nullopt,
            const std::optional<std::string>& label = std::nullopt
        );

        // Not copyable or movable: holds a PdfMemDocument and PdfSigningContext,
        // neither of which support copy or move themselves
        PdfRemoteSignDocumentSession(const PdfRemoteSignDocumentSession&) = delete;
        PdfRemoteSignDocumentSession& operator=(const PdfRemoteSignDocumentSession&) = delete;
        PdfRemoteSignDocumentSession(PdfRemoteSignDocumentSession&&) = delete;
        PdfRemoteSignDocumentSession& operator=(PdfRemoteSignDocumentSession&&) = delete;
        ~PdfRemoteSignDocumentSession();

        /**
         * @brief Start the signing process and compute the document hash to be signed remotely.
         * @return URL-encoded base64 of the hash that should be signed by a remote service.
         */
        std::string beginSigning();
        /**
         * @brief Finish the signing by injecting the remote signature and optional timestamp and DSS.
         * @param signedHash Base64-encoded signature/content returned by the remote signer.
         * @param base64Tsr Base64-encoded TimeStampResp (required for ADES_B_T, ADES_B_LT, ADES_B_LTA).
         * @param validationData Optional validation artifacts to embed into DSS.
         */
        void finishSigning(const std::string& signedHash, const std::string& base64Tsr, const std::optional<ValidationData>& validationData = std::nullopt);

        /**
         * @brief Start a DocTimeStamp (RFC3161) LTA update flow on the existing signed PDF.
         * @return Base64-encoded hash to be sent to the TSA.
         */
        std::string beginSigningLTA();
        /**
         * @brief Complete the DocTimeStamp flow by injecting the TSA token and optional DSS.
         * @param base64Tsr Base64-encoded TSR from TSA.
         * @param validationData Optional validation artifacts to embed into DSS.
         */
        void finishSigningLTA(const std::string& base64Tsr, const std::optional<ValidationData>& validationData);

        /**
         * @brief Prints current session state to stdout (for diagnostics)
         */
        void printState() const;
        /**
         * @brief Sets the timestamp token (base64 TSR) to be used in the session
         * @param responseTsrBase64 Base64-encoded timestamp response
         */
        void setTimestampToken(const std::string& responseTsrBase64);
        /**
         * @brief Extract the first CRL Distribution Point URL from a certificate or TSR (base64 DER input).
         * @throws std::runtime_error if no URL is found or parsing fails.
         */
        std::string getCrlFromCertificate(const std::string& base64Cert);

        /**
         * @brief Gets an OCSP request from a base64-encoded TSR and returns both the OCSP URL and the base64-encoded OCSP request.
         * @param base64Tsr The base64-encoded TSR (timestamp response)
         * @return std::pair<std::string, std::string> A pair containing (ocspUrl, base64_ocsp_request)
         * @throws std::runtime_error if any step fails
         */
        std::pair<std::string, std::string> getOCSPRequestFromCertificates(const std::string& base64Tsr);

        /**
         * @brief Gets an OCSP request from a base64-encoded TSR with AIA fallback support.
         * @param base64Tsr The base64-encoded TSR (timestamp response)
         * @param httpFetcher Function to fetch certificates from HTTP URLs, should return base64-encoded certificate
         * @return std::pair<std::string, std::string> A pair containing (ocspUrl, base64_ocsp_request)
         * @throws std::runtime_error if any step fails
         */
        std::pair<std::string, std::string> getOCSPRequestFromCertificatesWithFallback(const std::string& base64Tsr,
            std::function<std::string(const std::string&)> httpFetcher);

        /**
         * @brief Extracts the TSA signer certificate from a base64-encoded TSR.
         * @param base64Tsr The base64-encoded TSR (timestamp response)
         * @return std::string The base64 DER encoding of the signer certificate
         * @throws std::runtime_error on failure
         */
        std::string extractSignerCertFromTSR(const std::string& base64Tsr);

        /**
         * @brief Extracts the TSA issuer certificate from a base64-encoded TSR.
         * @param base64Tsr The base64-encoded TSR (timestamp response)
         * @return std::string The base64 DER encoding of the issuer certificate
         * @throws std::runtime_error on failure
         */
        std::string extractIssuerCertFromTSR(const std::string& base64Tsr);

        /**
         * @brief Extracts the OCSP responder URL from a certificate's AIA extension.
         * @param base64Cert The certificate encoded in base64
         * @param base64IssuerCert The issuer certificate encoded in base64
         * @return The OCSP responder URL as a string
         * @throws std::runtime_error if any step fails
         */
        std::string getOCSPFromCertificate(const std::string& base64Cert, const std::string& base64IssuerCert);

        /**
         * @brief Gets an OCSP request from base64-encoded certificates and returns it as base64.
         * @param base64Cert The certificate encoded in base64
         * @param base64IssuerCert The issuer certificate encoded in base64
         * @return std::string The base64-encoded OCSP request
         * @throws std::runtime_error if any step fails
         */
        std::string buildOCSPRequestFromCertificates(const std::string& base64Cert, const std::string& base64IssuerCert);

        /**
         * @brief Extracts the CA Issuers URL from a certificate's AIA extension.
         * @param base64Cert The certificate encoded in base64
         * @return The CA Issuers URL as a string
         * @throws std::runtime_error if no CA Issuers URL is found
         */
        std::string getCertificateIssuerUrlFromCertificate(const std::string& base64Cert);

    private:
        /**
         * @brief Create or update the DSS dictionary in the document with provided artifacts.
         */
        void createOrUpdateDSSCatalog(PdfMemDocument& doc, const ValidationData& validationData);
        /**
         * @brief Creates a stream object for a certificate
         */
        PdfObject& createCertificateStream(PdfMemDocument& doc, const std::string& certBase64);
        /**
         * @brief Creates a stream object for a CRL
         */
        PdfObject& createCRLStream(PdfMemDocument& doc, const std::string& crlBase64);
        /**
         * @brief Creates a stream object for an OCSP response
         */
        PdfObject& createOCSPStream(PdfMemDocument& doc, const std::string& ocspBase64);

        /**
         * @brief Attempts to extract issuer certificate from TSR, with AIA fallback.
         */
        std::string extractIssuerCertFromTSRWithFallback(const std::string& base64Tsr,
            std::function<std::string(const std::string&)> httpFetcher = nullptr);

        /**
         * @brief Decodes base64 (no newlines) into DER bytes
         */
        std::vector<unsigned char> ConvertBase64PEMtoDER(
            const std::optional<std::string>& base64PEM,
            const std::optional<std::string>& outputPath);
        /**
         * @brief Encodes raw buffer to base64 (no newlines)
         */
        std::string ToBase64(const charbuff& data);
        /**
         * @brief Decodes base64-encoded DSS hash into raw bytes
         */
        charbuff ConvertDSSHashToSignedHash(const std::string& DSSHash);
        /**
         * @brief Converts hex string to byte vector
         */
        std::vector<unsigned char> HexToBytes(const std::string& hex);
        /**
         * @brief Converts raw buffer to lowercase hex string
         */
        std::string ToHexString(const charbuff& data);
        /**
         * @brief URL-encodes a string (RFC3986 unreserved kept)
         */
        std::string UrlEncode(const std::string& value);
        /**
         * @brief Decodes base64 TSR and validates it parses as TS_RESP
         */
        std::string DecodeBase64Tsr(const std::string& base64Tsr);
        /**
         * @brief Extracts the PKCS#7 timeStampToken from a TS_RESP blob
         */
        std::string ExtractTimestampTokenFromTSR(const std::string& tsrData);

        std::string                                 _conformanceLevel;
        HashAlgorithm                               _hashAlgorithm;
        std::string                                 _documentInputPath;
        std::string                                 _documentOutputPath;
        std::string                                 _endCertificateBase64;
        std::vector<std::string>                    _certificateChainBase64;
        std::optional<std::string>                  _rootCertificateBase64;
        std::optional<std::string>                  _label;
        std::optional<std::string>                  _responseTsrBase64;
        std::optional<ValidationData>               _validationData;
        std::vector<unsigned char>                  _endCertificateDer;
        std::vector<std::vector<unsigned char>>     _certificateChainDer;
        std::vector<unsigned char>                  _rootCertificateDer;
        std::vector<unsigned char>                  _responseTsr;

        PdfMemDocument                              _doc;
        std::shared_ptr<FileStreamDevice>           _stream;
        PdfSignerCmsParams                          _cmsParams;
        PdfSigningContext                           _ctx;
        PdfSigningResults                           _results;
        PdfSignerId                                 _signerId;
        std::shared_ptr<PdfSignerCms>               _signer;

        // Members for LTA Signing Flow
        std::unique_ptr<PdfMemDocument>             _ltaDoc;
        std::unique_ptr<PdfSigningContext>          _ltaCtx;
        std::shared_ptr<PdfSigner>                  _ltaSigner;
        PdfSignerId                                 _ltaSignerId;
        PdfSigningResults                           _ltaResults;

        static HashAlgorithm hashAlgorithmFromOid(const std::string& oid);
        static const char* hashAlgorithmToString(HashAlgorithm alg);
    };

    /**
     * @brief Custom signer implementing RFC3161 DocTimeStamp behavior.
     *
     * Computes the correct hash over the ByteRange of the PDF and accepts an external
     * timestamp token to be embedded as the signature contents.
     */
    class PODOFO_API PdfDocTimeStampSigner : public PdfSigner {
    private:
        charbuff m_hashBuffer;
        std::shared_ptr<StreamDevice> m_device;
        bool m_useManualByteRange;

    public:
        PdfDocTimeStampSigner();
        /**
         * @brief Provides the underlying stream device to allow manual ByteRange hashing
         */
        void SetDevice(std::shared_ptr<StreamDevice> device);
        void Reset() override;
        void AppendData(const bufferview& data) override;
        void ComputeSignature(charbuff& contents, bool dryrun) override;
        void FetchIntermediateResult(charbuff& result) override;

    private:
        charbuff calculateCorrectHash();
        void ComputeSignatureDeferred(const bufferview& processedResult, charbuff& contents, bool dryrun) override;
        std::string GetSignatureFilter() const override;
        std::string GetSignatureSubFilter() const override;
        std::string GetSignatureType() const override;
        bool SkipBufferClear() const override;
    };

} // namespace PoDoFo

#endif // PDF_DSS_SIGNING_SESSION_H
