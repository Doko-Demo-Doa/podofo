#import <Foundation/Foundation.h>

@class PoDoFoSignatureContents;

NS_ASSUME_NONNULL_BEGIN

/// Result of -verifySignatureAtPath:error:.
typedef NS_ENUM(NSInteger, PoDoFoSignatureVerifyStatus) {
    /// The signature could not be checked. Missing or invalid PKCS7/CMS
    /// contents, unsupported signer info layout, or a malformed /ByteRange.
    PoDoFoSignatureVerifyStatusIndeterminate = 0,

    /// The signature is cryptographically invalid: the signed bytes were
    /// modified, or the signature doesn't correspond to the certificate.
    PoDoFoSignatureVerifyStatusInvalid = 1,

    /// The CMS signature is cryptographically valid over the signed bytes,
    /// but the /ByteRange doesn't reach the end of the input: the document
    /// has content that is not covered by the signature. The certificate
    /// trust status is indeterminate.
    PoDoFoSignatureVerifyStatusCryptoVerifiedPartialCoverage = 2,

    /// The CMS signature is cryptographically valid and the /ByteRange
    /// covers the whole input. The certificate trust status is indeterminate.
    PoDoFoSignatureVerifyStatusCryptoVerified = 3,
};

/// A signature AcroForm field — wraps PoDoFo's PdfSignature.
///
/// Obtain an instance from -[PoDoFoField asSignatureAndReturnError:].
///
/// The wrapper is non-owning: the underlying PdfSignature is owned by the
/// document's AcroForm, just like PoDoFoField.
@interface PoDoFoSignature : NSObject

/// True if the field has a signature value object with /Contents set.
@property (nonatomic, readonly) BOOL hasSignatureValue;

/// The signature handler filter, eg. "Adobe.PPKLite", or nil.
@property (nonatomic, readonly, nullable, copy) NSString *filter;

/// The signature sub-filter, eg. "ETSI.CAdES.detached", or nil.
@property (nonatomic, readonly, nullable, copy) NSString *subFilter;

/// The signature type, eg. "Sig" or "DocTimeStamp", or nil.
@property (nonatomic, readonly, nullable, copy) NSString *type;

/// The /Name entry (signer name as declared by the signing tool), or nil.
@property (nonatomic, readonly, nullable, copy) NSString *name;

/// The /Reason entry, or nil.
@property (nonatomic, readonly, nullable, copy) NSString *reason;

/// The /Location entry, or nil.
@property (nonatomic, readonly, nullable, copy) NSString *location;

/// The /ContactInfo entry, or nil.
@property (nonatomic, readonly, nullable, copy) NSString *contactInfo;

/// The /M entry (claimed signing date) in W3C format, or nil. This is the
/// date declared in the PDF dictionary, not a cryptographically attested
/// CMS signingTime.
@property (nonatomic, readonly, nullable, copy) NSString *signDate;

/// The /ByteRange array, or nil.
@property (nonatomic, readonly, nullable) NSArray<NSNumber *> *byteRange;

/// The raw /Prop_Build dictionary as a PDF string, or nil.
@property (nonatomic, readonly, nullable, copy) NSString *propBuild;

/// The raw CMS/PKCS7 bytes from /Contents, or nil.
@property (nonatomic, readonly, nullable) NSData *contents;

/// Convenience: decode /Contents and parse the CMS/PKCS7 blob. Returns nil
/// (with *error set) if the signature has no /Contents or it couldn't be parsed.
- (nullable PoDoFoSignatureContents *)parseContentsAndReturnError:(NSError **)error;

/// Cryptographically verifies this signature against the actual bytes of
/// the PDF file it belongs to (reads /ByteRange from the file at
/// documentPath directly — no need to extract the bytes yourself).
///
/// It performs no certificate trust validation, and it doesn't inspect the
/// document beyond the signed revision: a complete verification is a
/// document wide concern.
- (PoDoFoSignatureVerifyStatus)verifySignatureAtPath:(NSString *)documentPath error:(NSError **)error;

@end

NS_ASSUME_NONNULL_END
