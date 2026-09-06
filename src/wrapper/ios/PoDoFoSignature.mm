#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoSignature {
    PdfSignature *_signature;
    PoDoFoDocument *_document;
}

- (instancetype)initWithSignature:(PdfSignature &)signature document:(PoDoFoDocument *)document
{
    self = [super init];
    if (self) {
        _signature = &signature;
        _document = document;
    }
    return self;
}

- (PdfSignature *)podofo_signature
{
    return _signature;
}

- (BOOL)hasSignatureValue
{
    return _signature->HasSignatureValue();
}

- (nullable NSString *)filter
{
    auto value = _signature->GetFilter();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)subFilter
{
    auto value = _signature->GetSubFilter();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)type
{
    auto value = _signature->GetType();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)name
{
    auto value = _signature->GetSignerName();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)reason
{
    auto value = _signature->GetSignatureReason();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)location
{
    auto value = _signature->GetSignatureLocation();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)contactInfo
{
    auto value = _signature->GetContactInfo();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->GetString()).c_str()] : nil;
}

- (nullable NSString *)signDate
{
    auto value = _signature->GetSignatureDate();
    return value.has_value() ? [NSString stringWithUTF8String:std::string(value->ToStringW3C().GetString()).c_str()] : nil;
}

- (nullable NSArray<NSNumber *> *)byteRange
{
    auto value = _signature->GetByteRange();
    if (!value.has_value())
        return nil;

    NSMutableArray<NSNumber *> *result = [NSMutableArray arrayWithCapacity:value->GetSize()];
    for (unsigned i = 0; i < value->GetSize(); i++) {
        int64_t num = 0;
        value->TryGetAtAs((int)i, num);
        [result addObject:@(num)];
    }
    return result;
}

- (nullable NSString *)propBuild
{
    auto value = _signature->GetPropBuild();
    if (!value.has_value())
        return nil;

    try {
        PdfVariant variant(*value);
        std::string str;
        variant.ToString(str, PdfWriteFlags::None);
        return [NSString stringWithUTF8String:str.c_str()];
    } catch (const std::exception &) {
        return nil;
    }
}

- (nullable NSData *)contents
{
    charbuff contents;
    if (!_signature->TryGetContents(contents))
        return nil;
    return [NSData dataWithBytes:contents.data() length:contents.size()];
}

- (nullable PoDoFoSignatureContents *)parseContentsAndReturnError:(NSError **)error
{
    PdfSignatureContents parsed;
    if (!_signature->TryGetSignatureContents(parsed)) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Signature has no /Contents");
        return nil;
    }
    if (!parsed.IsValid()) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Failed to parse signature contents");
        return nil;
    }
    return [[PoDoFoSignatureContents alloc] initWithContents:parsed];
}

- (PoDoFoSignatureVerifyStatus)verifySignatureAtPath:(NSString *)documentPath error:(NSError **)error
{
    try {
        FileStreamDevice device(documentPath.UTF8String);
        PdfSignatureVerifyStatus status;
        _signature->TryVerifySignature(device, status);
        return (PoDoFoSignatureVerifyStatus)status;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return PoDoFoSignatureVerifyStatusIndeterminate;
    }
}

@end
