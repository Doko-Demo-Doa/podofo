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

  private final long nativeHandle;

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

  private native double nativeGetWidth(long handle);

  private native double nativeGetHeight(long handle);

  private native int nativeGetIndex(long handle);
}
