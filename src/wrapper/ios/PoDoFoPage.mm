#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoPage {
    PdfPage *_page;
    PoDoFoDocument *_document;
}

- (instancetype)initWithPage:(PdfPage &)page document:(PoDoFoDocument *)document
{
    self = [super init];
    if (self) {
        _page = &page;
        // Held only to keep the owning PoDoFoDocument (and thus the native
        // PdfMemDocument this page belongs to) alive for as long as this
        // page wrapper is — mirrors the ownership comment on the Java side
        // (PdfPage becomes invalid once the owning PdfDocument is closed);
        // this doesn't change that contract, it just avoids a *worse*
        // use-after-free where the ObjC document wrapper itself gets
        // deallocated first.
        _document = document;
    }
    return self;
}

- (PdfPage *)podofo_page
{
    return _page;
}

- (double)width
{
    return _page->GetRect().Width;
}

- (double)height
{
    return _page->GetRect().Height;
}

- (NSUInteger)index
{
    return (NSUInteger)_page->GetIndex();
}

- (NSArray<PoDoFoTextEntry *> *)extractText
{
    return [self extractTextInternal:nil];
}

- (NSArray<PoDoFoTextEntry *> *)extractTextMatchingPattern:(NSString *)regexPattern
{
    return [self extractTextInternal:regexPattern];
}

- (NSArray<PoDoFoTextEntry *> *)extractTextInternal:(nullable NSString *)pattern
{
    std::vector<PdfTextEntry> entries;
    try {
        if (pattern == nil)
            _page->ExtractTextTo(entries);
        else
            _page->ExtractTextTo(entries, pattern.UTF8String);
    } catch (const std::exception &) {
        return @[];
    }

    NSMutableArray<PoDoFoTextEntry *> *result = [NSMutableArray arrayWithCapacity:entries.size()];
    for (const auto &entry : entries) {
        PoDoFoTextEntry *textEntry = [[PoDoFoTextEntry alloc] initWithText:[NSString stringWithUTF8String:entry.Text.c_str()]
                                                                        page:(NSUInteger)entry.Page
                                                                           x:entry.X
                                                                           y:entry.Y
                                                                      length:entry.Length];
        [result addObject:textEntry];
    }
    return result;
}

- (NSUInteger)annotationCount
{
    return (NSUInteger)_page->GetAnnotations().GetCount();
}

- (nullable PoDoFoAnnotation *)annotationAtIndex:(NSUInteger)index error:(NSError **)error
{
    try {
        auto &annot = _page->GetAnnotations().GetAnnotAt((unsigned)index);
        return [[PoDoFoAnnotation alloc] initWithAnnotation:annot page:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

static bool tryGetAnnotationType(NSString *name, PdfAnnotationType &type)
{
    std::string n = name.UTF8String;
    if (n == "Text") type = PdfAnnotationType::Text;
    else if (n == "Link") type = PdfAnnotationType::Link;
    else if (n == "FreeText") type = PdfAnnotationType::FreeText;
    else if (n == "Line") type = PdfAnnotationType::Line;
    else if (n == "Square") type = PdfAnnotationType::Square;
    else if (n == "Circle") type = PdfAnnotationType::Circle;
    else if (n == "Highlight") type = PdfAnnotationType::Highlight;
    else if (n == "Underline") type = PdfAnnotationType::Underline;
    else if (n == "Squiggly") type = PdfAnnotationType::Squiggly;
    else if (n == "StrikeOut") type = PdfAnnotationType::StrikeOut;
    else if (n == "Stamp") type = PdfAnnotationType::Stamp;
    else if (n == "Ink") type = PdfAnnotationType::Ink;
    else if (n == "Popup") type = PdfAnnotationType::Popup;
    else return false;
    return true;
}

- (nullable PoDoFoAnnotation *)createAnnotationOfType:(NSString *)annotationType
                                                   rect:(CGRect)rect
                                                  error:(NSError **)error
{
    PdfAnnotationType type;
    if (!tryGetAnnotationType(annotationType, type)) {
        PoDoFoSetError(error, PoDoFoErrorInvalidArgument,
            [NSString stringWithFormat:@"Unknown or unsupported annotation type: %@", annotationType]);
        return nil;
    }

    try {
        auto &annot = _page->GetAnnotations().CreateAnnot(type, PoDoFo::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
        return [[PoDoFoAnnotation alloc] initWithAnnotation:annot page:self];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (NSInteger)rotation
{
    return (NSInteger)_page->GetRotation();
}

- (BOOL)setRotation:(NSInteger)rotation error:(NSError **)error
{
    try {
        _page->SetRotation((int)rotation);
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

static CGRect CGRectFromPoDoFoRect(const PoDoFo::Rect &rect)
{
    return CGRectMake(rect.X, rect.Y, rect.Width, rect.Height);
}

- (CGRect)mediaBox
{
    return CGRectFromPoDoFoRect(_page->GetMediaBox());
}

- (BOOL)setMediaBox:(CGRect)rect error:(NSError **)error
{
    try {
        _page->SetMediaBox(PoDoFo::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

- (CGRect)cropBox
{
    return CGRectFromPoDoFoRect(_page->GetCropBox());
}

- (BOOL)setCropBox:(CGRect)rect error:(NSError **)error
{
    try {
        _page->SetCropBox(PoDoFo::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

- (BOOL)moveToIndex:(NSUInteger)newIndex error:(NSError **)error
{
    try {
        return _page->MoveTo((unsigned)newIndex) ? YES : NO;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

@end
