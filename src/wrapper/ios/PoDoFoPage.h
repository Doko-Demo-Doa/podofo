#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@class PoDoFoTextEntry;
@class PoDoFoAnnotation;

NS_ASSUME_NONNULL_BEGIN

/// A single page within a PoDoFoDocument.
///
/// Instances are only ever obtained from -[PoDoFoDocument pageAtIndex:error:]
/// or -[PoDoFoDocument createPageWithWidth:height:error:] — there is no
/// public initializer. The underlying native page is owned by its parent
/// document's page tree (matching PoDoFo's own C++ ownership contract), not
/// by this object, so a PoDoFoPage becomes invalid once the owning
/// PoDoFoDocument is closed. There is deliberately no -close here.
@interface PoDoFoPage : NSObject

/// The page width in PDF units (from the page's MediaBox rect).
@property (nonatomic, readonly) double width;

/// The page height in PDF units (from the page's MediaBox rect).
@property (nonatomic, readonly) double height;

/// The 0-based index of this page within its document.
@property (nonatomic, readonly) NSUInteger index;

/// Extracts all text on this page, in PDF content-stream order (not
/// necessarily reading order for complex layouts).
- (NSArray<PoDoFoTextEntry *> *)extractText;

/// Extracts text on this page matching a regular expression.
/// @param regexPattern an ECMAScript-flavor regex pattern
- (NSArray<PoDoFoTextEntry *> *)extractTextMatchingPattern:(NSString *)regexPattern;

@property (nonatomic, readonly) NSUInteger annotationCount;

/// Returns the annotation at the given 0-based index. Owned by this
/// page's annotation collection, same lifetime hazard as
/// -[PoDoFoDocument pageAtIndex:error:].
- (nullable PoDoFoAnnotation *)annotationAtIndex:(NSUInteger)index error:(NSError **)error;

/// Creates a new annotation on this page.
/// @param annotationType one of PoDoFo's PdfAnnotationType names, eg.
///        "Text", "Link", "Highlight", "Square", "Line", "Popup"
- (nullable PoDoFoAnnotation *)createAnnotationOfType:(NSString *)annotationType
                                                  rect:(CGRect)rect
                                                 error:(NSError **)error;

@end

NS_ASSUME_NONNULL_END
