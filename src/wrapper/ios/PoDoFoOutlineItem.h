#import <Foundation/Foundation.h>

@class PoDoFoPage;

NS_ASSUME_NONNULL_BEGIN

/// A node in the document's outline (bookmark) tree — wraps PoDoFo's
/// PdfOutlineItem/PdfOutlines (the root returned by
/// -[PoDoFoDocument getOrCreateOutlines] is itself a PdfOutlines, which
/// extends PdfOutlineItem on the C++ side, so one class covers both).
///
/// Owned by the document's outline tree (same as PoDoFoPage) — no public
/// initializer, no close.
@interface PoDoFoOutlineItem : NSObject

@property (nonatomic, copy) NSString *title;

/// Creates a new child item, becoming this item's first child (or
/// appended after existing children).
- (nullable PoDoFoOutlineItem *)createChildWithTitle:(NSString *)title error:(NSError **)error;

/// Creates a new item on the same level, immediately following this one.
- (nullable PoDoFoOutlineItem *)createNextWithTitle:(NSString *)title error:(NSError **)error;

/// The first child item, or nil if this item has no children.
@property (nonatomic, readonly, nullable) PoDoFoOutlineItem *firstChild;

/// The next sibling item, or nil if this is the last on its level.
@property (nonatomic, readonly, nullable) PoDoFoOutlineItem *next;

/// The parent item, or nil if this is the top-level outlines root.
@property (nonatomic, readonly, nullable) PoDoFoOutlineItem *parent;

/// Points this item at a page, with the page scaled to fit the viewer
/// window (the common "jump to page" case). For finer-grained
/// destinations (specific rect/zoom) there is currently no binding.
- (void)setDestinationToPage:(PoDoFoPage *)page;

@end

NS_ASSUME_NONNULL_END
