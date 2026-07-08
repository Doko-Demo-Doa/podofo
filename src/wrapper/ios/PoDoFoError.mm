#import "PoDoFoInternal.h"

NSString * const PoDoFoErrorDomain = @"org.podofo.PoDoFo";

void PoDoFoSetErrorFromException(NSError * _Nullable * _Nullable error, const std::exception &e)
{
    if (error == nil)
        return;

    *error = [NSError errorWithDomain:PoDoFoErrorDomain
                                  code:PoDoFoErrorNativeException
                              userInfo:@{ NSLocalizedDescriptionKey: [NSString stringWithUTF8String:e.what()] ?: @"Unknown native error" }];
}

void PoDoFoSetError(NSError * _Nullable * _Nullable error, PoDoFoErrorCode code, NSString *message)
{
    if (error == nil)
        return;

    *error = [NSError errorWithDomain:PoDoFoErrorDomain
                                  code:code
                              userInfo:@{ NSLocalizedDescriptionKey: message }];
}
