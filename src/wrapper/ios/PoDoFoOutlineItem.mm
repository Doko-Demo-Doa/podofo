#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoOutlineItem {
    PdfOutlineItem *_item;
    PoDoFoDocument *_document;
}

- (instancetype)initWithOutlineItem:(PdfOutlineItem *)item document:(PoDoFoDocument *)document
{
    self = [super init];
    if (self) {
        _item = item;
        _document = document;
    }
    return self;
}

- (PdfOutlineItem *)podofo_outlineItem
{
    return _item;
}

- (NSString *)title
{
    return [NSString stringWithUTF8String:std::string(_item->GetTitle().GetString()).c_str()];
}

- (void)setTitle:(NSString *)title
{
    if (title == nil)
        [NSException raise:NSInvalidArgumentException format:@"title must not be nil"];
    _item->SetTitle(PdfString(title.UTF8String));
}

- (nullable PoDoFoOutlineItem *)createChildWithTitle:(NSString *)title error:(NSError **)error
{
    try {
        auto &child = _item->CreateChild(PdfString(title.UTF8String));
        return [[PoDoFoOutlineItem alloc] initWithOutlineItem:&child document:_document];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoOutlineItem *)createNextWithTitle:(NSString *)title error:(NSError **)error
{
    try {
        auto &next = _item->CreateNext(PdfString(title.UTF8String));
        return [[PoDoFoOutlineItem alloc] initWithOutlineItem:&next document:_document];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable PoDoFoOutlineItem *)firstChild
{
    auto *first = _item->First();
    return first == nullptr ? nil : [[PoDoFoOutlineItem alloc] initWithOutlineItem:first document:_document];
}

- (nullable PoDoFoOutlineItem *)next
{
    auto *next = _item->Next();
    return next == nullptr ? nil : [[PoDoFoOutlineItem alloc] initWithOutlineItem:next document:_document];
}

- (nullable PoDoFoOutlineItem *)parent
{
    auto *parent = _item->GetParentOutline();
    return parent == nullptr ? nil : [[PoDoFoOutlineItem alloc] initWithOutlineItem:parent document:_document];
}

- (void)setDestinationToPage:(PoDoFoPage *)page
{
    auto *podofoPage = page.podofo_page;
    // PdfDestination's constructors are private (friend-only); the public
    // way to obtain one is PdfDocument::CreateDestination().
    auto dest = podofoPage->GetDocument().CreateDestination();
    dest->SetDestination(*podofoPage, PdfDestinationFit::Fit);
    _item->SetDestination(*dest);
}

@end
