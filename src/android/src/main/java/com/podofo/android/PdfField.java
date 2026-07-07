package com.podofo.android;

/**
 * A single AcroForm field (text box, checkbox, radio button, push button,
 * combo/list box, or signature field) — wraps PoDoFo's {@code PdfField}
 * hierarchy. Rather than a Java class per field subclass, this is one
 * class for all field types; {@link #getText()}/{@link #setText(String)}
 * and {@link #isChecked()}/{@link #setChecked(boolean)} throw
 * {@link PoDoFoException} if called on a field of the wrong type (checked
 * natively via {@code dynamic_cast}).
 *
 * Owned by the document's AcroForm (same as {@link PdfPage}/{@link PdfFont})
 * — no public constructor, no close() of its own.
 */
public class PdfField {

  private final long nativeHandle;

  PdfField(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  /**
   * @return one of: PushButton, CheckBox, RadioButton, TextBox, ComboBox,
   *         ListBox, Signature, Unknown (matches PoDoFo's PdfFieldType names)
   */
  public String getFieldType() {
    return nativeGetFieldType(nativeHandle);
  }

  /**
   * @return the fully qualified field name (parent names joined with '.')
   */
  public String getFullName() {
    return nativeGetFullName(nativeHandle);
  }

  /**
   * @throws PoDoFoException if this field isn't a TextBox
   */
  public String getText() throws PoDoFoException {
    return nativeGetText(nativeHandle);
  }

  /**
   * @throws PoDoFoException if this field isn't a TextBox
   */
  public void setText(String text) throws PoDoFoException {
    nativeSetText(nativeHandle, text);
  }

  /**
   * @throws PoDoFoException if this field isn't a CheckBox/RadioButton
   */
  public boolean isChecked() throws PoDoFoException {
    return nativeIsChecked(nativeHandle);
  }

  /**
   * @throws PoDoFoException if this field isn't a CheckBox/RadioButton
   */
  public void setChecked(boolean checked) throws PoDoFoException {
    nativeSetChecked(nativeHandle, checked);
  }

  private native String nativeGetFieldType(long handle);

  private native String nativeGetFullName(long handle);

  private native String nativeGetText(long handle);

  private native void nativeSetText(long handle, String text);

  private native boolean nativeIsChecked(long handle);

  private native void nativeSetChecked(long handle, boolean checked);
}
