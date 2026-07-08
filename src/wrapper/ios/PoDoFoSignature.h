#import <Foundation/Foundation.h>

@class PoDoFoSignatureContents;

NS_ASSUME_NONNULL_BEGIN

/// Result of -verifySignatureWithData:/-verifySignatureAtPath:error:.
typedef NS_ENUM(NSInteger, PoDoFoSignatureVerifyStatus) {
    /// The signature could not be checked at all (missing /ByteRange,
    /// malformed CMS/PKCS7 data, I/O error reading the document, etc.).
    /// Distinct from Invalid — this means "unable to tell", not "checked,
    /// and it's wrong".
    PoDoFoSignatureVerifyStatusCouldNotVerify = 0,

    /// The signature does not match the given bytes, or is otherwise
    /// cryptographically invalid (eg. the document was modified after signing).
    PoDoFoSignatureVerifyStatusInvalid = 1,

    /// The signature is cryptographically valid over the given bytes. This
    /// does NOT mean the signer's certificate should be trusted: no
    /// certificate chain or revocation checking is performed.
    PoDoFoSignatureVerifyStatusValidNoTrust = 2,
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
/// Does NOT validate the signer's certificate chain of trust — no trust
/// store or revocation checking is performed. A ValidNoTrust result means
/// the signature is cryptographically self-consistent (the document
/// wasn't tampered with and the signature matches the embedded
/// certificate), not that the certificate should be trusted.
- (PoDoFoSignatureVerifyStatus)verifySignatureAtPath:(NSString *)documentPath error:(NSError **)error;

/// Cryptographically verifies this signature against caller-supplied
/// bytes — the exact document content covered by -byteRange, with the
/// /Contents entry itself excluded. Use -verifySignatureAtPath:error:
/// instead unless you already have these exact bytes for another reason.
///
/// Same trust caveat as -verifySignatureAtPath:error:.
- (PoDoFoSignatureVerifyStatus)verifySignatureWithData:(NSData *)signedData;

@end

NS_ASSUME_NONNULL_END
