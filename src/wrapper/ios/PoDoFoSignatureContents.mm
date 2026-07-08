#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoSignatureSignerInfo

- (instancetype)initWithSignerInfo:(const PdfSignatureSignerInfo &)info
{
    self = [super init];
    if (self) {
        _certificate = info.Certificate.empty() ? nil : [NSData dataWithBytes:info.Certificate.data() length:info.Certificate.size()];
        _subject = info.Subject.empty() ? nil : [NSString stringWithUTF8String:info.Subject.c_str()];
        _issuer = info.Issuer.empty() ? nil : [NSString stringWithUTF8String:info.Issuer.c_str()];
        _serial = info.Serial.empty() ? nil : [NSString stringWithUTF8String:info.Serial.c_str()];
        _signingTime = info.SigningTime.has_value()
            ? [NSString stringWithUTF8String:std::string(info.SigningTime->ToStringW3C().GetString()).c_str()]
            : nil;
    }
    return self;
}

@end

@implementation PoDoFoSignatureTimestampInfo

- (instancetype)initWithTimestampInfo:(const PdfSignatureTimestampInfo &)info
{
    self = [super init];
    if (self) {
        _genTime = [NSString stringWithUTF8String:std::string(info.GenTime.ToStringW3C().GetString()).c_str()];
        _tsaCertificate = (info.TsaCertificate.has_value() && !info.TsaCertificate->empty())
            ? [NSData dataWithBytes:info.TsaCertificate->data() length:info.TsaCertificate->size()]
            : nil;
        _tsaSubject = info.TsaSubject.empty() ? nil : [NSString stringWithUTF8String:info.TsaSubject.c_str()];
    }
    return self;
}

@end

@implementation PoDoFoSignatureContents

- (instancetype)initWithContents:(const PdfSignatureContents &)contents
{
    self = [super init];
    if (self) {
        NSMutableArray<PoDoFoSignatureSignerInfo *> *signerInfos = [NSMutableArray array];
        for (const auto &signer : contents.GetSignerInfos()) {
            [signerInfos addObject:[[PoDoFoSignatureSignerInfo alloc] initWithSignerInfo:signer]];
        }
        _signerInfos = signerInfos;

        PdfSignatureTimestampInfo ts;
        _timestampToken = contents.TryGetTimestampToken(ts) ? [[PoDoFoSignatureTimestampInfo alloc] initWithTimestampInfo:ts] : nil;
    }
    return self;
}

@end
