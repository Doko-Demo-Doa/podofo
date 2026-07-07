package com.podofo.android;

/**
 * A font usable with {@link PdfPainter#setFont(PdfFont, double)}, obtained
 * from {@link PdfDocument#getStandard14Font(String)}.
 *
 * Owned by its parent {@link PdfDocument} (same as {@link PdfPage}) — no
 * public constructor, no close() of its own, invalid once the document
 * that created it is closed.
 */
public class PdfFont {

  final long nativeHandle;

  PdfFont(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }
}
