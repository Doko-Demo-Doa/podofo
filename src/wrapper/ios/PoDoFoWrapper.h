#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// A remote PDF signing session — wraps PoDoFo's
/// PdfRemoteSignDocumentSession. Two-phase (hash-then-sign) CMS signing
/// flow with PAdES B-B/B-T/B-LT/B-LTA conformance levels, DSS/LTV
/// embedding, and RFC3161 LTA timestamping. This is a separate, narrower
/// binding from PoDoFoDocument (general-purpose document I/O); the two
/// don't interact.
@interface PoDoFoWrapper : NSObject

/// @param conformanceLevel one of ADES_B_B, ADES_B_T, ADES_B_LT, ADES_B_LTA
/// @param hashAlgorithm digest OID string (eg. 2.16.840.1.101.3.4.2.1 for SHA-256)
/// @param inputPath path to the input PDF file
/// @param outputPath path to save the signed PDF file
/// @param certificate the signing certificate, base64 DER
/// @param chainCertificates certificate chain, each item base64 DER
- (nullable instancetype)initWithConformanceLevel:(NSString *)conformanceLevel
                                     hashAlgorithm:(NSString *)hashAlgorithm
                                         inputPath:(NSString *)inputPath
                                        outputPath:(NSString *)outputPath
                                       certificate:(NSString *)certificate
                                 chainCertificates:(NSArray<NSString *> *)chainCertificates
                                             error:(NSError **)error;

@property (nonatomic, readonly, getter=isLoaded) BOOL loaded;

/// Prints the current session state to stdout (for diagnostics).
- (void)printState;

/// Sets the signature /Name entry. Call before calculateHashAndReturnError:.
- (BOOL)setSignerName:(NSString *)name error:(NSError **)error;

/// Sets the signature /Prop_Build/App/Name entry. Call before calculateHashAndReturnError:.
- (BOOL)setCreatingApplication:(NSString *)application error:(NSError **)error;

/// Sets the signature /Location entry. Call before calculateHashAndReturnError:.
- (BOOL)setSignatureLocation:(NSString *)location error:(NSError **)error;

/// Sets the signature /Reason entry. Call before calculateHashAndReturnError:.
- (BOOL)setSignatureReason:(NSString *)reason error:(NSError **)error;

/// Sets the signature /ContactInfo entry. Call before calculateHashAndReturnError:.
- (BOOL)setSignatureContactInfo:(NSString *)contactInfo error:(NSError **)error;

/// Starts the signing process and computes the document hash to be signed remotely.
- (nullable NSString *)calculateHashAndReturnError:(NSError **)error;

/// Finishes signing by injecting the remote signature and optional
/// timestamp/DSS material.
/// @param tsr base64-encoded TimeStampResp (required for ADES_B_T/B_LT/B_LTA)
/// @param certificates base64-encoded certificates to embed in the DSS dictionary
/// @param crls base64-encoded CRLs to embed in the DSS dictionary
/// @param ocsps base64-encoded OCSP responses to embed in the DSS dictionary
- (BOOL)finalizeSigningWithSignedHash:(NSString *)signedHash
                                   tsr:(nullable NSString *)tsr
                validationCertificates:(nullable NSArray<NSString *> *)certificates
                        validationCRLs:(nullable NSArray<NSString *> *)crls
                       validationOCSPs:(nullable NSArray<NSString *> *)ocsps
                                 error:(NSError **)error;

/// Begins the LTA (Long-Term Archive) DocTimeStamp flow. Call after a B-LT
/// signature has been created.
/// @return the base64-encoded hash to send to the Timestamping Authority
- (nullable NSString *)beginSigningLTAAndReturnError:(NSError **)error;

/// Completes the LTA DocTimeStamp flow with a timestamp response.
- (BOOL)finishSigningLTAWithTSR:(NSString *)tsr
          validationCertificates:(nullable NSArray<NSString *> *)certificates
                  validationCRLs:(nullable NSArray<NSString *> *)crls
                 validationOCSPs:(nullable NSArray<NSString *> *)ocsps
                           error:(NSError **)error;

/// Extracts the CRL distribution point URL from a base64-encoded certificate.
- (nullable NSString *)crlFromCertificate:(NSString *)base64Cert error:(NSError **)error;

/// Extracts the TSA signer certificate from a base64-encoded TSR. Returns the base64 DER encoding.
- (nullable NSString *)signerCertificateFromTSR:(NSString *)base64Tsr error:(NSError **)error;

/// Extracts the TSA issuer certificate from a base64-encoded TSR. Returns the base64 DER encoding.
- (nullable NSString *)issuerCertificateFromTSR:(NSString *)base64Tsr error:(NSError **)error;

/// Extracts the OCSP responder URL from a certificate's AIA extension.
- (nullable NSString *)ocspFromCertificate:(NSString *)base64Cert
                          issuerCertificate:(NSString *)base64IssuerCert
                                      error:(NSError **)error;

/// Builds an OCSP request from base64-encoded certificates, returned as base64.
- (nullable NSString *)buildOCSPRequestFromCertificate:(NSString *)base64Cert
                                      issuerCertificate:(NSString *)base64IssuerCert
                                                  error:(NSError **)error;

/// Extracts the CA Issuers URL from a certificate's AIA extension.
- (nullable NSString *)certificateIssuerURLFromCertificate:(NSString *)base64Cert error:(NSError **)error;

/// Frees the native session. Safe to call more than once.
- (void)close;

@end

NS_ASSUME_NONNULL_END
