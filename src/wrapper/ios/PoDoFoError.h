#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// Error domain used by every PoDoFo class. Plays the same role as
/// PoDoFoException on the Android/JNI side, but as an NSError rather than a
/// thrown exception — the idiomatic Cocoa pattern for recoverable errors.
extern NSString * const PoDoFoErrorDomain;

typedef NS_ENUM(NSInteger, PoDoFoErrorCode) {
    /// A PoDoFo C++ call threw; -localizedDescription carries what()/a
    /// generic message for non-std::exception throws.
    PoDoFoErrorNativeException = 1,

    /// An argument failed a precondition check before any native call was
    /// made (eg. a required NSString was nil).
    PoDoFoErrorInvalidArgument = 2,

    /// The receiver was used after being closed/invalidated (eg. a
    /// PoDoFoDocument method called after -close, or an object obtained
    /// from a document that has since been closed).
    PoDoFoErrorInvalidState = 3,
};

NS_ASSUME_NONNULL_END
