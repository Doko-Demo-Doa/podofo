#import "PoDoFoInternal.h"

@implementation PoDoFoTextEntry

- (instancetype)initWithText:(NSString *)text
                         page:(NSUInteger)page
                            x:(double)x
                            y:(double)y
                       length:(double)length
{
    self = [super init];
    if (self) {
        _text = [text copy];
        _page = page;
        _x = x;
        _y = y;
        _length = length;
    }
    return self;
}

@end
