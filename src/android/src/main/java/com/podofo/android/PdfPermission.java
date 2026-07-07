package com.podofo.android;

/**
 * Bitmask flags for {@link PdfDocument#setEncrypted(String, String, int)},
 * mirroring PoDoFo's {@code PdfPermissions} enum values exactly (these are
 * the actual bit positions defined by the PDF spec, not an
 * arbitrarily-assigned Java-side numbering) so they pass straight through
 * to native code unchanged.
 */
public final class PdfPermission {

  public static final int PRINT = 0x00000004;
  public static final int EDIT = 0x00000008;
  public static final int COPY = 0x00000010;
  public static final int EDIT_NOTES = 0x00000020;
  public static final int FILL_AND_SIGN = 0x00000100;
  public static final int ACCESSIBLE = 0x00000200;
  public static final int DOC_ASSEMBLY = 0x00000400;
  public static final int HIGH_PRINT = 0x00000800;

  /** Every permission granted — matches PoDoFo's own PdfPermissions::Default. */
  public static final int DEFAULT = PRINT | EDIT | COPY | EDIT_NOTES
      | FILL_AND_SIGN | ACCESSIBLE | DOC_ASSEMBLY | HIGH_PRINT;

  private PdfPermission() {
  }
}
