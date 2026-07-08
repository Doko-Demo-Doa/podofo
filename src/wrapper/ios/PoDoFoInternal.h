// Not a public header — not copied into the XCFramework's include/ dir (see
// scripts/ios/podofo-framework/build.sh). Shared only by the .mm files in
// this directory.
//
// Every public PoDoFo*.h header is Objective-C/Foundation only, so it can be
// imported from Swift and from plain Objective-C consumers without dragging
// in C++. The actual PoDoFo::* pointers each class wraps are declared here,
// in class extensions, and are only visible to code that imports this
// header — i.e. the .mm implementation files, which need to reach into a
// sibling class's native pointer (eg. PoDoFoPainter needs the raw
// PoDoFo::PdfPage* inside a PoDoFoPage to call PdfPainter::SetCanvas()).
// This plays the same role the reinterpret_cast<PoDoFo::X*>(handle) calls
// play in src/wrapper/podofo_jni.cpp, but type-safe.

#import "PoDoFoDocument.h"
#import "PoDoFoPage.h"
#import "PoDoFoFont.h"
#import "PoDoFoImage.h"
#import "PoDoFoField.h"
#import "PoDoFoSignature.h"
#import "PoDoFoAnnotation.h"
#import "PoDoFoOutlineItem.h"
#import "PoDoFoTextEntry.h"
#import "PoDoFoSignatureContents.h"
#import "PoDoFoError.h"

#include <podofo/podofo.h>

NS_ASSUME_NONNULL_BEGIN

/// Populates *error from a caught C++ exception. Every bridge method wraps
/// its native call(s) in a try/catch and calls this in the catch block,
/// mirroring the try/catch-then-throwJavaException pattern in every JNI
/// function in podofo_jni.cpp.
void PoDoFoSetErrorFromException(NSError * _Nullable * _Nullable error, const std::exception &e);
void PoDoFoSetError(NSError * _Nullable * _Nullable error, PoDoFoErrorCode code, NSString *message);

@interface PoDoFoDocument ()
@property (nonatomic, readonly, nullable) PoDoFo::PdfMemDocument *podofo_document;
@end

@interface PoDoFoPage ()
- (instancetype)initWithPage:(PoDoFo::PdfPage &)page document:(PoDoFoDocument *)document;
@property (nonatomic, readonly) PoDoFo::PdfPage *podofo_page;
@end

@interface PoDoFoFont ()
- (instancetype)initWithFont:(PoDoFo::PdfFont &)font document:(PoDoFoDocument *)document;
@property (nonatomic, readonly) PoDoFo::PdfFont *podofo_font;
@end

@interface PoDoFoImage ()
- (instancetype)initWithImage:(std::unique_ptr<PoDoFo::PdfImage>)image;
@property (nonatomic, readonly, nullable) PoDoFo::PdfImage *podofo_image;
@end

@interface PoDoFoField ()
- (instancetype)initWithField:(PoDoFo::PdfField &)field document:(PoDoFoDocument *)document;
@property (nonatomic, readonly) PoDoFo::PdfField *podofo_field;
@property (nonatomic, readonly) PoDoFoDocument *podofo_document;
@end

@interface PoDoFoSignature ()
- (instancetype)initWithSignature:(PoDoFo::PdfSignature &)signature document:(PoDoFoDocument *)document;
@property (nonatomic, readonly) PoDoFo::PdfSignature *podofo_signature;
@end

@interface PoDoFoAnnotation ()
- (instancetype)initWithAnnotation:(PoDoFo::PdfAnnotation &)annotation page:(PoDoFoPage *)page;
@property (nonatomic, readonly) PoDoFo::PdfAnnotation *podofo_annotation;
@end

@interface PoDoFoOutlineItem ()
- (instancetype)initWithOutlineItem:(PoDoFo::PdfOutlineItem *)item document:(PoDoFoDocument *)document;
@property (nonatomic, readonly, nullable) PoDoFo::PdfOutlineItem *podofo_outlineItem;
@end

@interface PoDoFoTextEntry ()
- (instancetype)initWithText:(NSString *)text
                         page:(NSUInteger)page
                            x:(double)x
                            y:(double)y
                       length:(double)length;
@end

@interface PoDoFoSignatureSignerInfo ()
- (instancetype)initWithSignerInfo:(const PoDoFo::PdfSignatureSignerInfo &)info;
@end

@interface PoDoFoSignatureTimestampInfo ()
- (instancetype)initWithTimestampInfo:(const PoDoFo::PdfSignatureTimestampInfo &)info;
@end

@interface PoDoFoSignatureContents ()
- (instancetype)initWithContents:(const PoDoFo::PdfSignatureContents &)contents;
@end

NS_ASSUME_NONNULL_END
