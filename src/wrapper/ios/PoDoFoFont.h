#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// A font usable with -[PoDoFoPainter setFont:size:], obtained from
/// -[PoDoFoDocument standard14FontNamed:error:] or
/// -[PoDoFoDocument fontFromFile:error:].
///
/// Owned by its parent PoDoFoDocument (same as PoDoFoPage) — no public
/// initializer, no close of its own, invalid once the document that
/// created it is closed.
@interface PoDoFoFont : NSObject
@end

NS_ASSUME_NONNULL_END
