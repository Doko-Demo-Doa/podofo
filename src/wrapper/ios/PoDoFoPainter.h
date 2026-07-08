#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@class PoDoFoPage;
@class PoDoFoFont;
@class PoDoFoImage;

NS_ASSUME_NONNULL_BEGIN

/// Draws text and simple shapes onto a PoDoFoPage — wraps PoDoFo's
/// PdfPainter. Only a basic slice of the C++ API is bound so far:
/// lines/rectangles/circles, RGB stroking/non-stroking color, and text
/// drawn with a PoDoFoFont.
///
/// Usage:
/// ```
/// PoDoFoPainter *painter = [[PoDoFoPainter alloc] initWithPage:page error:&error];
/// [painter setFont:font size:12];
/// [painter drawText:@"Hello" atX:50 y:700];
/// [painter finishDrawing];
/// [painter close];
/// ```
///
/// -finishDrawing must be called before -close — matches PoDoFo's own
/// contract that a painting session has to be explicitly finished before
/// the page's content stream is valid.
@interface PoDoFoPainter : NSObject

/// Creates a painter bound to the given page, ready to draw.
- (nullable instancetype)initWithPage:(PoDoFoPage *)page error:(NSError **)error;

/// Sets the font and size used by subsequent -drawText:atX:y: calls. Must
/// be called at least once before drawing any text.
- (void)setFont:(PoDoFoFont *)font size:(double)fontSize;

/// Draws a single line of text. -setFont:size: must be called first.
- (void)drawText:(NSString *)text atX:(double)x y:(double)y;

/// Draws an image at its natural size.
- (void)drawImage:(PoDoFoImage *)image atX:(double)x y:(double)y;

/// Draws an image, scaled by the given factors.
- (void)drawImage:(PoDoFoImage *)image atX:(double)x y:(double)y scaleX:(double)scaleX scaleY:(double)scaleY;

/// Strokes a line with the current stroking color and line settings.
- (void)drawLineFromX:(double)x1 y:(double)y1 toX:(double)x2 y:(double)y2;

/// @param fill true to fill using the non-zero winding rule, false to stroke the outline only
- (void)drawRect:(CGRect)rect fill:(BOOL)fill;

/// @param fill true to fill using the non-zero winding rule, false to stroke the outline only
- (void)drawCircleAtX:(double)x y:(double)y radius:(double)radius fill:(BOOL)fill;

/// Sets the color used for stroking (line/outline) operations.
- (void)setStrokingColorRed:(double)red green:(double)green blue:(double)blue;

/// Sets the color used for non-stroking (fill/text) operations.
- (void)setNonStrokingColorRed:(double)red green:(double)green blue:(double)blue;

/// Pushes the current graphics state (matches the PDF 'q' operator).
- (void)save;

/// Pops the graphics state (matches the PDF 'Q' operator).
- (void)restore;

/// Finishes drawing onto the page. Must be called once drawing is
/// complete, before -close.
- (void)finishDrawing;

/// Safe to call more than once.
- (void)close;

@end

NS_ASSUME_NONNULL_END
