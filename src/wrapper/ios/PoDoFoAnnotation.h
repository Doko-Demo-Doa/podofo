#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

/// A single annotation on a PoDoFoPage (text note, link, highlight,
/// square, line, etc.) — wraps PoDoFo's PdfAnnotation base class. One
/// class for every annotation type, same approach as PoDoFoField: only
/// the base API (position/size, contents, type) is bound, not
/// type-specific behavior (eg. a Line annotation's endpoints, a Link's
/// destination).
///
/// Owned by its parent page's annotation collection (same as
/// PoDoFoPage/PoDoFoFont) — no public initializer, no close.
@interface PoDoFoAnnotation : NSObject

/// One of PoDoFo's PdfAnnotationType names, eg. "Text", "Link",
/// "Highlight", "Square", "Line", "Popup", "Unknown".
@property (nonatomic, readonly, copy) NSString *annotationType;

/// In PDF page coordinates.
@property (nonatomic) CGRect rect;

/// The annotation's text content (eg. a sticky note's body), or nil if unset.
@property (nonatomic, copy, nullable) NSString *contents;

@end

NS_ASSUME_NONNULL_END
