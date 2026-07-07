package com.podofo.android;

/**
 * A single page within a {@link PdfDocument}.
 *
 * Instances are only ever obtained from {@link PdfDocument#getPage(int)} or
 * {@link PdfDocument#createPage(double, double)} — there is no public
 * constructor. The underlying native page is owned by its parent document's
 * page tree (matching PoDoFo's own C++ ownership contract), not by this
 * object, so a PdfPage becomes invalid once the owning PdfDocument is
 * closed. There is deliberately no close()/AutoCloseable here.
 */
public class PdfPage {

  // Package-visible, not private: PdfPainter.setCanvas() needs the raw
  // handle to pass to PoDoFo::PdfPainter::SetCanvas() (PdfPage IS-A
  // PdfCanvas on the C++ side).
  final long nativeHandle;

  PdfPage(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  /**
   * @return the page width in PDF units (from the page's MediaBox rect)
   */
  public double getWidth() {
    return nativeGetWidth(nativeHandle);
  }

  /**
   * @return the page height in PDF units (from the page's MediaBox rect)
   */
  public double getHeight() {
    return nativeGetHeight(nativeHandle);
  }

  /**
   * @return the 0-based index of this page within its document
   */
  public int getIndex() {
    return nativeGetIndex(nativeHandle);
  }

  /**
   * Extracts all text on this page, in PDF content-stream order (not
   * necessarily reading order for complex layouts).
   */
  public PdfTextEntry[] extractText() {
    return nativeExtractText(nativeHandle, null);
  }

  /**
   * Extracts text on this page matching a regular expression.
   *
   * @param regexPattern an ECMAScript-flavor regex pattern
   */
  public PdfTextEntry[] extractText(String regexPattern) {
    if (regexPattern == null) {
      throw new IllegalArgumentException("regexPattern must not be null");
    }
    return nativeExtractText(nativeHandle, regexPattern);
  }

  /**
   * @return the number of annotations on this page
   */
  public int getAnnotationCount() {
    return nativeGetAnnotationCount(nativeHandle);
  }

  /**
   * Returns the annotation at the given 0-based index. Owned by this
   * page's annotation collection, same lifetime hazard as
   * {@link PdfDocument#getPage(int)}.
   *
   * @throws PoDoFoException if index is out of range
   */
  public PdfAnnotation getAnnotationAt(int index) throws PoDoFoException {
    long annotHandle = nativeGetAnnotationAt(nativeHandle, index);
    if (annotHandle == 0) {
      throw new PoDoFoException("Failed to get annotation at index " + index);
    }
    return new PdfAnnotation(annotHandle);
  }

  /**
   * Creates a new annotation on this page.
   *
   * @param annotationType one of PoDoFo's PdfAnnotationType names, eg.
   *                       "Text", "Link", "Highlight", "Square", "Line",
   *                       "Popup"
   * @throws PoDoFoException if annotationType is unknown or creation fails
   */
  public PdfAnnotation createAnnotation(String annotationType, double x, double y, double width, double height)
      throws PoDoFoException {
    if (annotationType == null) {
      throw new IllegalArgumentException("annotationType must not be null");
    }
    long annotHandle = nativeCreateAnnotation(nativeHandle, annotationType, x, y, width, height);
    if (annotHandle == 0) {
      throw new PoDoFoException("Failed to create annotation: " + annotationType);
    }
    return new PdfAnnotation(annotHandle);
  }

  private native double nativeGetWidth(long handle);

  private native double nativeGetHeight(long handle);

  private native int nativeGetIndex(long handle);

  private native PdfTextEntry[] nativeExtractText(long handle, String pattern);

  private native int nativeGetAnnotationCount(long handle);

  private native long nativeGetAnnotationAt(long handle, int index);

  private native long nativeCreateAnnotation(long handle, String annotationType, double x, double y, double width, double height);
}
