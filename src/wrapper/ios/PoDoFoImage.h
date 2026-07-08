#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// An image embedded in a PoDoFoDocument (wraps PoDoFo's PdfImage/
/// PdfXObject), obtained from -[PoDoFoDocument imageFromBuffer:error:] and
/// drawn onto a page via -[PoDoFoPainter drawImage:atX:y:].
///
/// Unlike PoDoFoPage/PoDoFoField/PoDoFoAnnotation, this is an *owning*
/// wrapper (like PoDoFoPainter) — PdfDocument::CreateImage() on the C++
/// side returns a unique_ptr the caller must keep alive, rather than
/// something owned by an existing document-internal collection. -close
/// only deletes this lightweight wrapper object, though — the actual image
/// data is already embedded in the document's object graph by the time
/// -[PoDoFoDocument imageFromBuffer:error:] returns, so closing (or never
/// explicitly closing) this object doesn't affect a page that has already
/// drawn it.
@interface PoDoFoImage : NSObject

@property (nonatomic, readonly) NSUInteger width;
@property (nonatomic, readonly) NSUInteger height;

/// Safe to call more than once.
- (void)close;

@end

NS_ASSUME_NONNULL_END
