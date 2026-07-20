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

/// The page's clockwise rotation in degrees, normalized to one of
/// 0, 90, 180, 270 (from the page's /Rotate entry).
@property (nonatomic, readonly) NSInteger rotation;

/// Sets the page's /Rotate entry.
/// @param rotation clockwise rotation in degrees; must be a multiple of 90
///                 (negative values are normalized by PoDoFo)
- (BOOL)setRotation:(NSInteger)rotation error:(NSError **)error;

/// The page's /MediaBox as a rect in PDF units.
@property (nonatomic, readonly) CGRect mediaBox;

/// Sets the page's /MediaBox, resizing the page.
- (BOOL)setMediaBox:(CGRect)rect error:(NSError **)error;

/// The page's /CropBox (visible region) in PDF units, or the /MediaBox if no /CropBox is set.
@property (nonatomic, readonly) CGRect cropBox;

/// Sets the page's /CropBox (visible region).
- (BOOL)setCropBox:(CGRect)rect error:(NSError **)error;

/// Moves this page to a new 0-based index within its parent document's page tree.
/// @return YES if the page was actually moved, NO if it was already at newIndex.
- (BOOL)moveToIndex:(NSUInteger)newIndex error:(NSError **)error;

@end

NS_ASSUME_NONNULL_END
