#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoImage {
    std::unique_ptr<PdfImage> _image;
}

- (instancetype)initWithImage:(std::unique_ptr<PdfImage>)image
{
    self = [super init];
    if (self) {
        _image = std::move(image);
    }
    return self;
}

- (PdfImage *)podofo_image
{
    return _image.get();
}

- (NSUInteger)width
{
    return _image ? (NSUInteger)_image->GetWidth() : 0;
}

- (NSUInteger)height
{
    return _image ? (NSUInteger)_image->GetHeight() : 0;
}

- (void)close
{
    _image.reset();
}

- (void)dealloc
{
    [self close];
}

@end
