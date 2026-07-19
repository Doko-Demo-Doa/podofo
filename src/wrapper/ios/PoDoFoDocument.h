#import <Foundation/Foundation.h>
#import "PoDoFoPermission.h"

@class PoDoFoPage;
@class PoDoFoFont;
@class PoDoFoImage;
@class PoDoFoField;
@class PoDoFoOutlineItem;

NS_ASSUME_NONNULL_BEGIN

/// General-purpose PDF document I/O and page access — wraps PoDoFo's
/// PdfMemDocument (create/load/save, page count, page access/insertion/
/// removal, AcroForm fields, outlines, fonts, images, encryption, basic
/// metadata). This is a separate, narrower binding from PoDoFoWrapper
/// (which wraps the remote-signing session specifically); the two don't
/// interact.
@interface PoDoFoDocument : NSObject

/// Creates a new, empty PDF document.
+ (instancetype)documentByCreatingNew;

/// Loads a PDF document from a file path.
+ (nullable instancetype)documentWithContentsOfFile:(NSString *)path error:(NSError **)error;

/// Loads a PDF document from a file path.
/// @param password optional password for encrypted documents
+ (nullable instancetype)documentWithContentsOfFile:(NSString *)path
                                            password:(nullable NSString *)password
                                               error:(NSError **)error;

/// Saves the complete document to a file.
- (BOOL)saveToFile:(NSString *)path error:(NSError **)error;

/// Frees the native document. Any PoDoFoPage/PoDoFoField/PoDoFoAnnotation/
/// PoDoFoOutlineItem/PoDoFoFont instances obtained from this document
/// become invalid after this call. Safe to call more than once.
- (void)close;

@property (nonatomic, readonly) NSUInteger pageCount;

/// Returns the page at the given 0-based index. The returned PoDoFoPage is
/// owned by this document's page tree, same as in the underlying C++ API —
/// it becomes invalid once this document is closed.
- (nullable PoDoFoPage *)pageAtIndex:(NSUInteger)index error:(NSError **)error;

/// Creates a new page and appends it to the end of the document.
/// @param width page width in PDF units
/// @param height page height in PDF units
- (nullable PoDoFoPage *)createPageWithWidth:(double)width height:(double)height error:(NSError **)error;

/// Removes the page at the given 0-based index. Any PoDoFoPage instances
/// already obtained for later pages become invalid — matches PoDoFo's own
/// C++ semantics (indices shift after removal).
- (BOOL)removePageAtIndex:(NSUInteger)index error:(NSError **)error;

/// Gets one of the 14 PDF "standard" fonts (Helvetica, Times, Courier,
/// Symbol, ZapfDingbats and their bold/italic variants) — always renderable
/// by any PDF viewer, no font embedding needed. This works without
/// Fontconfig (PODOFO_WITH_FONTMANAGER is off for this build), unlike
/// PoDoFo's by-name SearchFont().
/// @param name one of: TimesRoman, TimesItalic, TimesBold, TimesBoldItalic,
///             Helvetica, HelveticaOblique, HelveticaBold,
///             HelveticaBoldOblique, Courier, CourierOblique, CourierBold,
///             CourierBoldOblique, Symbol, ZapfDingbats (matches PoDoFo's
///             own PdfStandard14FontType names)
- (nullable PoDoFoFont *)standard14FontNamed:(NSString *)name error:(NSError **)error;

/// Loads a font from a font file (TTF/OTF) for use with
/// -[PoDoFoPainter setFont:size:]. Counterpart to
/// -standard14FontNamed:error: for callers that want a specific embedded
/// font rather than one of the 14 built-ins — there is no by-name system
/// font search available (needs Fontconfig, unavailable in this build).
- (nullable PoDoFoFont *)fontFromFile:(NSString *)fontFilePath error:(NSError **)error;

/// Loads a font (TTF/OTF) already in memory — the counterpart to
/// -fontFromFile:error: for fonts bundled as app resources (eg. an asset
/// in the app bundle) that would otherwise need to be copied out to a
/// temporary file first just to get a filesystem path.
- (nullable PoDoFoFont *)fontFromBuffer:(NSData *)data error:(NSError **)error;

/// Decodes an encoded image (JPEG/PNG/etc.) from a buffer and embeds it in
/// the document, ready to draw via -[PoDoFoPainter drawImage:atX:y:].
- (nullable PoDoFoImage *)imageFromBuffer:(NSData *)data error:(NSError **)error;

/// Encrypts the document (AES-256, PDF 2.0 revision 6 — PoDoFo's own
/// default algorithm) with PoDoFoPermissionDefault permissions. Takes
/// effect on the next -saveToFile:error:.
/// @param userPassword required to open the document at all; pass "" for no open password
/// @param ownerPassword required to change permissions/remove protection
- (void)setEncryptedWithUserPassword:(NSString *)userPassword ownerPassword:(NSString *)ownerPassword;

/// Encrypts the document (AES-256, PDF 2.0 revision 6 — PoDoFo's own
/// default algorithm) with the given permissions. Takes effect on the next
/// -saveToFile:error:. There is currently no binding for choosing a
/// different algorithm/key length or legacy RC4 encryption.
- (void)setEncryptedWithUserPassword:(NSString *)userPassword
                        ownerPassword:(NSString *)ownerPassword
                          permissions:(PoDoFoPermission)permissions;

/// True if this document was loaded from (or has been set to become,
/// pending save) an encrypted file.
@property (nonatomic, readonly, getter=isEncrypted) BOOL encrypted;

/// Number of AcroForm fields in this document (0 if there is no AcroForm yet).
@property (nonatomic, readonly) NSUInteger fieldCount;

/// Returns the AcroForm field at the given 0-based index. Owned by the
/// document's AcroForm, same lifetime hazard as -pageAtIndex:error:.
- (nullable PoDoFoField *)fieldAtIndex:(NSUInteger)index error:(NSError **)error;

/// Creates a new text box field, creating the document's AcroForm first if
/// it doesn't exist yet.
- (nullable PoDoFoField *)createTextBoxNamed:(NSString *)name error:(NSError **)error;

/// Creates a new checkbox field, creating the document's AcroForm first if
/// it doesn't exist yet.
- (nullable PoDoFoField *)createCheckBoxNamed:(NSString *)name error:(NSError **)error;

/// The root of the document's outline (bookmark) tree, creating it if it
/// doesn't exist yet.
- (PoDoFoOutlineItem *)getOrCreateOutlines;

@property (nonatomic, copy, nullable) NSString *title;
@property (nonatomic, copy, nullable) NSString *author;
@property (nonatomic, copy, nullable) NSString *subject;
@property (nonatomic, copy, nullable) NSString *creator;

@end

NS_ASSUME_NONNULL_END
