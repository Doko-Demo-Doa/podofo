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

  /**
   * @return the page's clockwise rotation in degrees, normalized to one of
   *         0, 90, 180, 270 (from the page's /Rotate entry)
   */
  public int getRotation() {
    return nativeGetRotation(nativeHandle);
  }

  /**
   * Sets the page's /Rotate entry.
   *
   * @param rotation clockwise rotation in degrees; must be a multiple of 90
   *                 (negative values are normalized by PoDoFo)
   * @throws PoDoFoException if rotation isn't a multiple of 90
   */
  public void setRotation(int rotation) throws PoDoFoException {
    nativeSetRotation(nativeHandle, rotation);
  }

  /**
   * @return the page's /MediaBox as {@code {x, y, width, height}} in PDF units
   */
  public double[] getMediaBox() {
    return nativeGetMediaBox(nativeHandle);
  }

  /**
   * Sets the page's /MediaBox, resizing the page.
   *
   * @param x      new MediaBox origin X
   * @param y      new MediaBox origin Y
   * @param width  new MediaBox width
   * @param height new MediaBox height
   */
  public void setMediaBox(double x, double y, double width, double height) {
    nativeSetMediaBox(nativeHandle, x, y, width, height);
  }

  /**
   * @return the page's /CropBox (visible region) as {@code {x, y, width, height}}
   *         in PDF units, or the /MediaBox if no /CropBox is set
   */
  public double[] getCropBox() {
    return nativeGetCropBox(nativeHandle);
  }

  /**
   * Sets the page's /CropBox (visible region).
   */
  public void setCropBox(double x, double y, double width, double height) {
    nativeSetCropBox(nativeHandle, x, y, width, height);
  }

  /**
   * Moves this page to a new 0-based index within its parent document's
   * page tree. Any {@link PdfPage} instances obtained for other pages become
   * invalid - their indices shift, matching
   * {@link PdfDocument#removePageAt(int)} semantics.
   *
   * @return true if the page was actually moved, false if it was already at
   *         {@code newIndex}
   * @throws PoDoFoException if {@code newIndex} is out of range
   */
  public boolean moveTo(int newIndex) throws PoDoFoException {
    return nativeMoveTo(nativeHandle, newIndex);
  }

  private native double nativeGetWidth(long handle);

  private native double nativeGetHeight(long handle);

  private native int nativeGetIndex(long handle);

  private native PdfTextEntry[] nativeExtractText(long handle, String pattern);

  private native int nativeGetAnnotationCount(long handle);

  private native long nativeGetAnnotationAt(long handle, int index);

  private native long nativeCreateAnnotation(long handle, String annotationType, double x, double y, double width, double height);

  private native int nativeGetRotation(long handle);

  private native void nativeSetRotation(long handle, int rotation);

  private native double[] nativeGetMediaBox(long handle);

  private native void nativeSetMediaBox(long handle, double x, double y, double width, double height);

  private native double[] nativeGetCropBox(long handle);

  private native void nativeSetCropBox(long handle, double x, double y, double width, double height);

  private native boolean nativeMoveTo(long handle, int newIndex);
}
