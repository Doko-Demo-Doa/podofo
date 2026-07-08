// SPDX-FileCopyrightText: 2026 OpenCode Contributors
// SPDX-License-Identifier: LGPL-2.0-or-later OR MPL-2.0

#ifndef PDF_SIGNATURE_CONTENTS_H
#define PDF_SIGNATURE_CONTENTS_H

#include "PdfDeclarations.h"
#include "PdfDate.h"

namespace PoDoFo {

/// Information about a CMS/PKCS7 signer extracted from a signature /Contents blob.
struct PODOFO_API PdfSignatureSignerInfo
{
    /// DER-encoded signer certificate.
    charbuff Certificate;

    /// X.509 subject name (RFC 2253 style).
    std::string Subject;

    /// X.509 issuer name (RFC 2253 style).
    std::string Issuer;

    /// Serial number as a hexadecimal string.
    std::string Serial;

    /// CMS authenticated signingTime attribute, if present.
    nullable<PdfDate> SigningTime;
};

/// Information about an embedded RFC3161 timestamp token.
struct PODOFO_API PdfSignatureTimestampInfo
{
    /// genTime from the TSTInfo.
    PdfDate GenTime;

    /// DER-encoded TSA certificate, if embedded in the token.
    nullable<charbuff> TsaCertificate;

    /// TSA subject name, if available.
    std::string TsaSubject;
};

/// Result of PdfSignatureContents::VerifySignature().
enum class PdfSignatureVerifyStatus : uint8_t
{
    /// The signature could not be checked at all: no parsed CMS/PKCS7
    /// contents to verify against, or the data couldn't be processed.
    /// This is distinct from Invalid — it means "unable to tell", not
    /// "checked, and it's wrong".
    CouldNotVerify = 0,

    /// The CMS signature does not match the given bytes, or is otherwise
    /// cryptographically invalid (eg. the document was modified after
    /// signing, or the signature doesn't correspond to the certificate).
    Invalid,

    /// The CMS signature is cryptographically valid over the given bytes.
    ///
    /// This does NOT mean the signer's certificate should be trusted: no
    /// certificate chain or revocation checking is performed. Read this as
    /// "the document wasn't tampered with, and the signature matches the
    /// embedded certificate" — not as "this signature should be trusted".
    /// Validating the certificate chain against a trust store is a separate,
    /// unimplemented concern.
    ValidNoTrust,
};

/// Parser for the raw CMS/PKCS7 data stored in a PDF signature /Contents entry.
/// This class extracts signer information and embedded RFC3161 timestamp tokens
/// without performing cryptographic verification.
class PODOFO_API PdfSignatureContents final
{
public:
    PdfSignatureContents();

    /// Parse the given raw signature contents.
    /// @param contents raw bytes from the PDF /Contents entry
    explicit PdfSignatureContents(const bufferview& contents);

    PdfSignatureContents(const PdfSignatureContents&) = delete;
    PdfSignatureContents(PdfSignatureContents&&) = default;
    PdfSignatureContents& operator=(const PdfSignatureContents&) = delete;
    PdfSignatureContents& operator=(PdfSignatureContents&&) = default;

    /// Parse the given raw signature contents.
    /// @param contents raw bytes from the PDF /Contents entry
    /// @returns true if the contents could be parsed as CMS/PKCS7
    bool TryParse(const bufferview& contents);

    /// Returns true if the contents were parsed successfully.
    bool IsValid() const { return m_valid; }

    /// Get all signer infos found in the CMS message.
    const std::vector<PdfSignatureSignerInfo>& GetSignerInfos() const { return m_signerInfos; }

    /// Try to extract an embedded RFC3161 timestamp token
    /// (signatureTimeStampToken unsigned attribute).
    /// @param info output structure that will receive the timestamp info
    /// @returns true if a timestamp token was found and parsed
    bool TryGetTimestampToken(PdfSignatureTimestampInfo& info) const;

    /// Cryptographically verifies the parsed CMS signature against the exact
    /// bytes that were supposedly signed — a PDF's /ByteRange content, with
    /// the /Contents entry itself excluded (see PdfSignature::TryVerifySignature
    /// for a convenience that extracts this from a document automatically).
    /// Does NOT validate the signer's certificate chain of trust — see
    /// PdfSignatureVerifyStatus.
    ///
    /// Thread-safe: this is a const method with no shared mutable state. Each
    /// call re-parses the retained raw CMS bytes into its own local OpenSSL
    /// objects (BIO/PKCS7), used and freed entirely within the call, so
    /// concurrent calls — on the same instance or different ones — never
    /// contend or race, as long as no other thread is concurrently calling
    /// TryParse() on the same instance (which does mutate it).
    ///
    /// @param signedData the exact document bytes covered by /ByteRange
    PdfSignatureVerifyStatus VerifySignature(const bufferview& signedData) const;

private:
    bool m_valid;
    std::vector<PdfSignatureSignerInfo> m_signerInfos;
    charbuff m_timestampToken;
    // Retained (not just parsed-and-discarded, unlike the original design)
    // so VerifySignature() can re-parse into its own local, call-scoped
    // PKCS7* on every call, rather than needing to store a live OpenSSL
    // object as a class member.
    charbuff m_rawContents;
};

}

#endif // PDF_SIGNATURE_CONTENTS_H
