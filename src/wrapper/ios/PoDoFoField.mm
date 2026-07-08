#import "PoDoFoInternal.h"

using namespace PoDoFo;

@implementation PoDoFoField {
    PdfField *_field;
}

- (instancetype)initWithField:(PdfField &)field document:(PoDoFoDocument *)document
{
    self = [super init];
    if (self) {
        _field = &field;
        _podofo_document = document;
    }
    return self;
}

- (PdfField *)podofo_field
{
    return _field;
}

- (NSString *)fieldType
{
    switch (_field->GetType()) {
        case PdfFieldType::PushButton: return @"PushButton";
        case PdfFieldType::CheckBox: return @"CheckBox";
        case PdfFieldType::RadioButton: return @"RadioButton";
        case PdfFieldType::TextBox: return @"TextBox";
        case PdfFieldType::ComboBox: return @"ComboBox";
        case PdfFieldType::ListBox: return @"ListBox";
        case PdfFieldType::Signature: return @"Signature";
        default: return @"Unknown";
    }
}

- (NSString *)fullName
{
    try {
        return [NSString stringWithUTF8String:_field->GetFullName().c_str()];
    } catch (const std::exception &) {
        return @"";
    }
}

- (nullable NSString *)textAndReturnError:(NSError **)error
{
    auto *textBox = dynamic_cast<PdfTextBox *>(_field);
    if (textBox == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a TextBox");
        return nil;
    }
    auto text = textBox->GetText();
    return text.has_value() ? [NSString stringWithUTF8String:std::string(text.value().GetString()).c_str()] : nil;
}

- (BOOL)setText:(nullable NSString *)text error:(NSError **)error
{
    auto *textBox = dynamic_cast<PdfTextBox *>(_field);
    if (textBox == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a TextBox");
        return NO;
    }
    if (text == nil)
        textBox->SetText(nullptr);
    else
        textBox->SetText(PdfString(text.UTF8String));
    return YES;
}

- (BOOL)getChecked:(BOOL *)checked error:(NSError **)error
{
    auto *toggle = dynamic_cast<PdfToggleButton *>(_field);
    if (toggle == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a CheckBox/RadioButton");
        return NO;
    }
    if (checked != NULL)
        *checked = toggle->IsChecked();
    return YES;
}

- (BOOL)setChecked:(BOOL)checked error:(NSError **)error
{
    auto *toggle = dynamic_cast<PdfToggleButton *>(_field);
    if (toggle == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a CheckBox/RadioButton");
        return NO;
    }
    toggle->SetChecked(checked);
    return YES;
}

- (nullable PoDoFoSignature *)asSignatureAndReturnError:(NSError **)error
{
    if (![self.fieldType isEqualToString:@"Signature"]) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a Signature field");
        return nil;
    }
    auto *signature = dynamic_cast<PdfSignature *>(_field);
    if (signature == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Field is not a Signature field");
        return nil;
    }
    return [[PoDoFoSignature alloc] initWithSignature:*signature document:_podofo_document];
}

@end
