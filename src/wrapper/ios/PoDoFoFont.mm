#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoFont {
    PdfFont *_font;
    PoDoFoDocument *_document;
}

- (instancetype)initWithFont:(PdfFont &)font document:(PoDoFoDocument *)document
{
    self = [super init];
    if (self) {
        _font = &font;
        _document = document;
    }
    return self;
}

- (PdfFont *)podofo_font
{
    return _font;
}

@end
