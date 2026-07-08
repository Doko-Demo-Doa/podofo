#import "PoDoFoInternal.h"
#import "PoDoFoPainter.h"

using namespace PoDoFo;

@implementation PoDoFoPainter {
    PdfPainter *_painter;
}

- (nullable instancetype)initWithPage:(PoDoFoPage *)page error:(NSError **)error
{
    self = [super init];
    if (self) {
        try {
            _painter = new PdfPainter();
        } catch (const std::exception &e) {
            PoDoFoSetErrorFromException(error, e);
            return nil;
        }

        try {
            _painter->SetCanvas(*page.podofo_page);
        } catch (const std::exception &e) {
            delete _painter;
            _painter = nullptr;
            PoDoFoSetErrorFromException(error, e);
            return nil;
        }
    }
    return self;
}

- (void)setFont:(PoDoFoFont *)font size:(double)fontSize
{
    _painter->TextState.SetFont(*font.podofo_font, fontSize);
}

- (void)drawText:(NSString *)text atX:(double)x y:(double)y
{
    _painter->DrawText(text.UTF8String, x, y);
}

- (void)drawImage:(PoDoFoImage *)image atX:(double)x y:(double)y
{
    [self drawImage:image atX:x y:y scaleX:1.0 scaleY:1.0];
}

- (void)drawImage:(PoDoFoImage *)image atX:(double)x y:(double)y scaleX:(double)scaleX scaleY:(double)scaleY
{
    _painter->DrawImage(*image.podofo_image, x, y, scaleX, scaleY);
}

- (void)drawLineFromX:(double)x1 y:(double)y1 toX:(double)x2 y:(double)y2
{
    _painter->DrawLine(x1, y1, x2, y2);
}

- (void)drawRect:(CGRect)rect fill:(BOOL)fill
{
    auto mode = fill ? PdfPathDrawMode::Fill : PdfPathDrawMode::Stroke;
    _painter->DrawRectangle(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, mode);
}

- (void)drawCircleAtX:(double)x y:(double)y radius:(double)radius fill:(BOOL)fill
{
    auto mode = fill ? PdfPathDrawMode::Fill : PdfPathDrawMode::Stroke;
    _painter->DrawCircle(x, y, radius, mode);
}

- (void)setStrokingColorRed:(double)red green:(double)green blue:(double)blue
{
    _painter->GraphicsState.SetStrokingColor(PdfColor(red, green, blue));
}

- (void)setNonStrokingColorRed:(double)red green:(double)green blue:(double)blue
{
    _painter->GraphicsState.SetNonStrokingColor(PdfColor(red, green, blue));
}

- (void)save
{
    _painter->Save();
}

- (void)restore
{
    _painter->Restore();
}

- (void)finishDrawing
{
    _painter->FinishDrawing();
}

- (void)close
{
    if (_painter != nullptr) {
        // ~PdfPainter() is noexcept(false): it can throw if FinishDrawing()
        // wasn't called first. Swallow rather than let a C++ exception
        // escape out of what's meant to be a cleanup call.
        try {
            delete _painter;
        } catch (const std::exception &) {
        }
        _painter = nullptr;
    }
}

- (void)dealloc
{
    [self close];
}

@end
