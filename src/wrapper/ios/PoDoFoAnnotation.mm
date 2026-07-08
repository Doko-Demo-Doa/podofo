#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoAnnotation {
    PdfAnnotation *_annotation;
    PoDoFoPage *_page;
}

- (instancetype)initWithAnnotation:(PdfAnnotation &)annotation page:(PoDoFoPage *)page
{
    self = [super init];
    if (self) {
        _annotation = &annotation;
        _page = page;
    }
    return self;
}

- (PdfAnnotation *)podofo_annotation
{
    return _annotation;
}

- (NSString *)annotationType
{
    switch (_annotation->GetType()) {
        case PdfAnnotationType::Text: return @"Text";
        case PdfAnnotationType::Link: return @"Link";
        case PdfAnnotationType::FreeText: return @"FreeText";
        case PdfAnnotationType::Line: return @"Line";
        case PdfAnnotationType::Square: return @"Square";
        case PdfAnnotationType::Circle: return @"Circle";
        case PdfAnnotationType::Polygon: return @"Polygon";
        case PdfAnnotationType::PolyLine: return @"PolyLine";
        case PdfAnnotationType::Highlight: return @"Highlight";
        case PdfAnnotationType::Underline: return @"Underline";
        case PdfAnnotationType::Squiggly: return @"Squiggly";
        case PdfAnnotationType::StrikeOut: return @"StrikeOut";
        case PdfAnnotationType::Stamp: return @"Stamp";
        case PdfAnnotationType::Caret: return @"Caret";
        case PdfAnnotationType::Ink: return @"Ink";
        case PdfAnnotationType::Popup: return @"Popup";
        case PdfAnnotationType::FileAttachement: return @"FileAttachement";
        case PdfAnnotationType::Sound: return @"Sound";
        case PdfAnnotationType::Movie: return @"Movie";
        case PdfAnnotationType::Widget: return @"Widget";
        case PdfAnnotationType::Screen: return @"Screen";
        case PdfAnnotationType::PrinterMark: return @"PrinterMark";
        case PdfAnnotationType::TrapNet: return @"TrapNet";
        case PdfAnnotationType::Watermark: return @"Watermark";
        case PdfAnnotationType::Model3D: return @"Model3D";
        case PdfAnnotationType::RichMedia: return @"RichMedia";
        case PdfAnnotationType::WebMedia: return @"WebMedia";
        case PdfAnnotationType::Redact: return @"Redact";
        case PdfAnnotationType::Projection: return @"Projection";
        default: return @"Unknown";
    }
}

- (CGRect)rect
{
    auto rect = _annotation->GetRect();
    return CGRectMake(rect.X, rect.Y, rect.Width, rect.Height);
}

- (void)setRect:(CGRect)rect
{
    _annotation->SetRect(PoDoFo::Rect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
}

- (nullable NSString *)contents
{
    auto contents = _annotation->GetContents();
    return contents.has_value() ? [NSString stringWithUTF8String:std::string(contents.value().GetString()).c_str()] : nil;
}

- (void)setContents:(nullable NSString *)contents
{
    if (contents == nil)
        _annotation->SetContents(nullptr);
    else
        _annotation->SetContents(PdfString(contents.UTF8String));
}

@end
