#import <Foundation/Foundation.h>

@class PoDoFoSignature;

NS_ASSUME_NONNULL_BEGIN

/// A single AcroForm field (text box, checkbox, radio button, push button,
/// combo/list box, or signature field) — wraps PoDoFo's PdfField
/// hierarchy. Rather than a class per field subclass, this is one class
/// for all field types; -textAndReturnError:/-setText:error: and
/// -isCheckedAndReturnError:/-setChecked:error: fail (returning an error)
/// if called on a field of the wrong type (checked natively via
/// dynamic_cast).
///
/// Owned by the document's AcroForm (same as PoDoFoPage/PoDoFoFont) — no
/// public initializer, no close of its own.
@interface PoDoFoField : NSObject

/// One of: PushButton, CheckBox, RadioButton, TextBox, ComboBox, ListBox,
/// Signature, Unknown (matches PoDoFo's PdfFieldType names).
@property (nonatomic, readonly, copy) NSString *fieldType;

/// The fully qualified field name (parent names joined with '.').
@property (nonatomic, readonly, copy) NSString *fullName;

/// Fails if this field isn't a TextBox.
- (nullable NSString *)textAndReturnError:(NSError **)error;

/// Fails if this field isn't a TextBox.
- (BOOL)setText:(nullable NSString *)text error:(NSError **)error;

/// Fails if this field isn't a CheckBox/RadioButton.
- (BOOL)getChecked:(BOOL *)checked error:(NSError **)error;

/// Fails if this field isn't a CheckBox/RadioButton.
- (BOOL)setChecked:(BOOL)checked error:(NSError **)error;

/// Fails if this field isn't a Signature field.
- (nullable PoDoFoSignature *)asSignatureAndReturnError:(NSError **)error;

@end

NS_ASSUME_NONNULL_END
