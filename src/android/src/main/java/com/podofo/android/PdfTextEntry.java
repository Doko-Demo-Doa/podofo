package com.podofo.android;

/**
 * A single piece of text extracted from a {@link PdfPage} via
 * {@link PdfPage#extractText()}/{@link PdfPage#extractText(String)},
 * mirroring PoDoFo's C++ {@code PdfTextEntry} struct.
 *
 * Position (x, y) is in PDF page coordinates (origin bottom-left).
 */
public class PdfTextEntry {

  public final String text;
  public final int page;
  public final double x;
  public final double y;
  public final double length;

  /** Constructed from native code; not intended to be built directly. */
  PdfTextEntry(String text, int page, double x, double y, double length) {
    this.text = text;
    this.page = page;
    this.x = x;
    this.y = y;
    this.length = length;
  }
}
