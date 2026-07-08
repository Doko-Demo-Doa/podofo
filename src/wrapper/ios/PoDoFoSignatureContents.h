#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// Information about a CMS/PKCS7 signer extracted from a signature
/// /Contents blob.
@interface PoDoFoSignatureSignerInfo : NSObject

/// DER-encoded signer certificate.
@property (nonatomic, readonly, nullable) NSData *certificate;

/// X.509 subject name (RFC 2253 style).
@property (nonatomic, readonly, nullable, copy) NSString *subject;

/// X.509 issuer name (RFC 2253 style).
@property (nonatomic, readonly, nullable, copy) NSString *issuer;

/// Certificate serial number as hex.
@property (nonatomic, readonly, nullable, copy) NSString *serial;

/// CMS authenticated signingTime attribute in W3C format, or nil. Note:
/// PoDoFo's default PAdES-B signer intentionally omits this attribute.
@property (nonatomic, readonly, nullable, copy) NSString *signingTime;

@end

/// Information about an embedded RFC3161 timestamp token.
@interface PoDoFoSignatureTimestampInfo : NSObject

/// genTime from the TSTInfo in W3C format.
@property (nonatomic, readonly, copy) NSString *genTime;

/// DER-encoded TSA certificate, if embedded in the token.
@property (nonatomic, readonly, nullable) NSData *tsaCertificate;

/// TSA subject name, if available.
@property (nonatomic, readonly, nullable, copy) NSString *tsaSubject;

@end

/// Parsed CMS/PKCS7 contents of a PDF digital signature.
///
/// Created by -[PoDoFoSignature parseContentsAndReturnError:].
@interface PoDoFoSignatureContents : NSObject

@property (nonatomic, readonly, copy) NSArray<PoDoFoSignatureSignerInfo *> *signerInfos;

/// The embedded RFC3161 timestamp token, or nil if absent.
@property (nonatomic, readonly, nullable) PoDoFoSignatureTimestampInfo *timestampToken;

@end

NS_ASSUME_NONNULL_END
