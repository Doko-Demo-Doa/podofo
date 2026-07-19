#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoDocument {
    PdfMemDocument *_doc;
}

- (instancetype)initWithNativeDocument:(PdfMemDocument *)doc
{
    self = [super init];
    if (self) {
        _doc = doc;
    }
    return self;
}

+ (instancetype)documentByCreatingNew
{
    return [[self alloc] initWithNativeDocument:new PdfMemDocument()];
}

+ (nullable instancetype)documentWithContentsOfFile:(NSString *)path error:(NSError **)error
{
    return [self documentWithContentsOfFile:path password:nil error:error];
}

+ (nullable instancetype)documentWithContentsOfFile:(NSString *)path
                                            password:(nullable NSString *)password
                                               error:(NSError **)error
{
    auto *doc = new PdfMemDocument();
    try {
        doc->Load(path.UTF8String, password != nil ? password.UTF8String : "");
        return [[self alloc] initWithNativeDocument:doc];
    } catch (const std::exception &e) {
        delete doc;
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (PdfMemDocument *)podofo_document
{
    return _doc;
}

- (BOOL)checkOpen:(NSError **)error
{
    if (_doc == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Document is closed");
        return NO;
    }
    return YES;
}

- (BOOL)saveToFile:(NSString *)path error:(NSError **)error
{
    if (![self checkOpen:error])
        return NO;
    try {
        _doc->Save(path.UTF8String);
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

- (void)close
{
    if (_doc != nullptr) {
        delete _doc;
        _doc = nullptr;
    }
}

- (void)dealloc
{
    [self close];
}

- (NSUInteger)pageCount
{
    if (_doc == nullptr)
        return 0;
    return (NSUInteger)_doc->GetPages().GetCount();
}

- (nullable PoDoFoPage *)pageAtIndex:(NSUInteger)index error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto &page = _doc->GetPages().GetPageAt((unsigned)index);
        return [[PoDoFoPage alloc] initWithPage:page document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoPage *)createPageWithWidth:(double)width height:(double)height error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto &page = _doc->GetPages().CreatePage(PoDoFo::Rect(0, 0, width, height));
        return [[PoDoFoPage alloc] initWithPage:page document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (BOOL)removePageAtIndex:(NSUInteger)index error:(NSError **)error
{
    if (![self checkOpen:error])
        return NO;
    try {
        _doc->GetPages().RemovePageAt((unsigned)index);
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

static bool tryGetStandard14FontType(NSString *name, PdfStandard14FontType &fontType)
{
    std::string n = name.UTF8String;
    if (n == "TimesRoman") fontType = PdfStandard14FontType::TimesRoman;
    else if (n == "TimesItalic") fontType = PdfStandard14FontType::TimesItalic;
    else if (n == "TimesBold") fontType = PdfStandard14FontType::TimesBold;
    else if (n == "TimesBoldItalic") fontType = PdfStandard14FontType::TimesBoldItalic;
    else if (n == "Helvetica") fontType = PdfStandard14FontType::Helvetica;
    else if (n == "HelveticaOblique") fontType = PdfStandard14FontType::HelveticaOblique;
    else if (n == "HelveticaBold") fontType = PdfStandard14FontType::HelveticaBold;
    else if (n == "HelveticaBoldOblique") fontType = PdfStandard14FontType::HelveticaBoldOblique;
    else if (n == "Courier") fontType = PdfStandard14FontType::Courier;
    else if (n == "CourierOblique") fontType = PdfStandard14FontType::CourierOblique;
    else if (n == "CourierBold") fontType = PdfStandard14FontType::CourierBold;
    else if (n == "CourierBoldOblique") fontType = PdfStandard14FontType::CourierBoldOblique;
    else if (n == "Symbol") fontType = PdfStandard14FontType::Symbol;
    else if (n == "ZapfDingbats") fontType = PdfStandard14FontType::ZapfDingbats;
    else return false;
    return true;
}

- (nullable PoDoFoFont *)standard14FontNamed:(NSString *)name error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;

    PdfStandard14FontType fontType;
    if (!tryGetStandard14FontType(name, fontType)) {
        PoDoFoSetError(error, PoDoFoErrorInvalidArgument, [NSString stringWithFormat:@"Unknown standard font: %@", name]);
        return nil;
    }

    try {
        auto &font = _doc->GetFonts().GetStandard14Font(fontType);
        return [[PoDoFoFont alloc] initWithFont:font document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoFont *)fontFromFile:(NSString *)fontFilePath error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto &font = _doc->GetFonts().GetOrCreateFont(fontFilePath.UTF8String);
        return [[PoDoFoFont alloc] initWithFont:font document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoFont *)fontFromBuffer:(NSData *)data error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        bufferview buffer(reinterpret_cast<const char *>(data.bytes), (size_t)data.length);
        auto &font = _doc->GetFonts().GetOrCreateFontFromBuffer(buffer);
        return [[PoDoFoFont alloc] initWithFont:font document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoImage *)imageFromBuffer:(NSData *)data error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto image = _doc->CreateImage();
        bufferview buffer(reinterpret_cast<const char *>(data.bytes), (size_t)data.length);
        image->LoadFromBuffer(buffer);
        return [[PoDoFoImage alloc] initWithImage:std::move(image)];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (void)setEncryptedWithUserPassword:(NSString *)userPassword ownerPassword:(NSString *)ownerPassword
{
    [self setEncryptedWithUserPassword:userPassword ownerPassword:ownerPassword permissions:PoDoFoPermissionDefault];
}

- (void)setEncryptedWithUserPassword:(NSString *)userPassword
                        ownerPassword:(NSString *)ownerPassword
                          permissions:(PoDoFoPermission)permissions
{
    if (_doc == nullptr)
        return;
    _doc->SetEncrypted(userPassword.UTF8String, ownerPassword.UTF8String, (PdfPermissions)permissions);
}

- (BOOL)isEncrypted
{
    if (_doc == nullptr)
        return NO;
    return _doc->IsEncrypted();
}

- (NSUInteger)fieldCount
{
    if (_doc == nullptr)
        return 0;
    auto *form = _doc->GetAcroForm();
    return form == nullptr ? 0 : (NSUInteger)form->GetFieldCount();
}

- (nullable PoDoFoField *)fieldAtIndex:(NSUInteger)index error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto &form = _doc->GetOrCreateAcroForm();
        auto &field = form.GetFieldAt((unsigned)index);
        return [[PoDoFoField alloc] initWithField:field document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoField *)createFieldNamed:(NSString *)name type:(PdfFieldType)fieldType error:(NSError **)error
{
    if (![self checkOpen:error])
        return nil;
    try {
        auto &form = _doc->GetOrCreateAcroForm();
        auto &field = form.CreateField(name.UTF8String, fieldType);
        return [[PoDoFoField alloc] initWithField:field document:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoField *)createTextBoxNamed:(NSString *)name error:(NSError **)error
{
    return [self createFieldNamed:name type:PdfFieldType::TextBox error:error];
}

- (nullable PoDoFoField *)createCheckBoxNamed:(NSString *)name error:(NSError **)error
{
    return [self createFieldNamed:name type:PdfFieldType::CheckBox error:error];
}

- (PoDoFoOutlineItem *)getOrCreateOutlines
{
    auto &outlines = _doc->GetOrCreateOutlines();
    return [[PoDoFoOutlineItem alloc] initWithOutlineItem:&outlines document:self];
}

- (nullable NSString *)title
{
    if (_doc == nullptr)
        return nil;
    auto title = _doc->GetMetadata().GetTitle();
    return title.has_value() ? [NSString stringWithUTF8String:std::string(title.value().GetString()).c_str()] : nil;
}

- (void)setTitle:(nullable NSString *)title
{
    if (_doc == nullptr)
        return;
    if (title == nil)
        _doc->GetMetadata().SetTitle(nullptr);
    else
        _doc->GetMetadata().SetTitle(PdfString(title.UTF8String));
}

- (nullable NSString *)author
{
    if (_doc == nullptr)
        return nil;
    auto author = _doc->GetMetadata().GetAuthor();
    return author.has_value() ? [NSString stringWithUTF8String:std::string(author.value().GetString()).c_str()] : nil;
}

- (void)setAuthor:(nullable NSString *)author
{
    if (_doc == nullptr)
        return;
    if (author == nil)
        _doc->GetMetadata().SetAuthor(nullptr);
    else
        _doc->GetMetadata().SetAuthor(PdfString(author.UTF8String));
}

- (nullable NSString *)subject
{
    if (_doc == nullptr)
        return nil;
    auto subject = _doc->GetMetadata().GetSubject();
    return subject.has_value() ? [NSString stringWithUTF8String:std::string(subject.value().GetString()).c_str()] : nil;
}

- (void)setSubject:(nullable NSString *)subject
{
    if (_doc == nullptr)
        return;
    if (subject == nil)
        _doc->GetMetadata().SetSubject(nullptr);
    else
        _doc->GetMetadata().SetSubject(PdfString(subject.UTF8String));
}

- (nullable NSString *)creator
{
    if (_doc == nullptr)
        return nil;
    auto creator = _doc->GetMetadata().GetCreator();
    return creator.has_value() ? [NSString stringWithUTF8String:std::string(creator.value().GetString()).c_str()] : nil;
}

- (void)setCreator:(nullable NSString *)creator
{
    if (_doc == nullptr)
        return;
    if (creator == nil)
        _doc->GetMetadata().SetCreator(nullptr);
    else
        _doc->GetMetadata().SetCreator(PdfString(creator.UTF8String));
}

@end
