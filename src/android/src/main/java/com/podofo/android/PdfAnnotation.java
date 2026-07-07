package com.podofo.android;

/**
 * A single annotation on a {@link PdfPage} (text note, link, highlight,
 * square, line, etc.) — wraps PoDoFo's {@code PdfAnnotation} base class.
 * One Java class for every annotation type, same approach as
 * {@link PdfField}: only the base API (position/size, contents, type) is
 * bound so far, not type-specific behavior (e.g. a Line annotation's
 * endpoints, a Link's destination).
 *
 * Owned by its parent page's annotation collection (same as
 * {@link PdfPage}/{@link PdfFont}) — no public constructor, no close().
 */
public class PdfAnnotation {

  private final long nativeHandle;

  PdfAnnotation(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  /**
   * @return one of PoDoFo's PdfAnnotationType names, eg. "Text", "Link",
   *         "Highlight", "Square", "Line", "Popup", "Unknown"
   */
  public String getAnnotationType() {
    return nativeGetAnnotationType(nativeHandle);
  }

  /**
   * @return {@code {x, y, width, height}} in PDF page coordinates
   */
  public double[] getRect() {
    return nativeGetRect(nativeHandle);
  }

  public void setRect(double x, double y, double width, double height) {
    nativeSetRect(nativeHandle, x, y, width, height);
  }

  /**
   * @return the annotation's text content (eg. a sticky note's body), or
   *         null if unset
   */
  public String getContents() {
    return nativeGetContents(nativeHandle);
  }

  public void setContents(String contents) {
    nativeSetContents(nativeHandle, contents);
  }

  private native String nativeGetAnnotationType(long handle);

  private native double[] nativeGetRect(long handle);

  private native void nativeSetRect(long handle, double x, double y, double width, double height);

  private native String nativeGetContents(long handle);

  private native void nativeSetContents(long handle, String contents);
}
