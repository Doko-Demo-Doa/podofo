#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// A single piece of text extracted from a PoDoFoPage via
/// -extractText/-extractTextMatchingPattern:, mirroring PoDoFo's C++
/// PdfTextEntry struct.
///
/// Position (x, y) is in PDF page coordinates (origin bottom-left).
@interface PoDoFoTextEntry : NSObject

@property (nonatomic, readonly, copy) NSString *text;
@property (nonatomic, readonly) NSUInteger page;
@property (nonatomic, readonly) double x;
@property (nonatomic, readonly) double y;
@property (nonatomic, readonly) double length;

@end

NS_ASSUME_NONNULL_END
