package com.podofo.android;

import android.util.Log;

/**
 * An image embedded in a {@link PdfDocument} (wraps PoDoFo's
 * {@code PdfImage}/{@code PdfXObject}), obtained from
 * {@link PdfDocument#createImageFromBuffer(byte[])} and drawn onto a page
 * via {@link PdfPainter#drawImage(PdfImage, double, double)}.
 *
 * Unlike {@link PdfPage}/{@link PdfField}/{@link PdfAnnotation}, this is an
 * <b>owning</b> wrapper (like {@link PdfPainter}) — {@code PdfDocument.CreateImage()}
 * on the C++ side returns a {@code unique_ptr} the caller must keep alive,
 * rather than something owned by an existing document-internal collection.
 * {@link #close()} only deletes this lightweight wrapper object, though —
 * the actual image data is already embedded in the document's object graph
 * by the time {@link PdfDocument#createImageFromBuffer(byte[])} returns, so
 * closing (or never explicitly closing) this object doesn't affect a page
 * that has already drawn it.
 */
public class PdfImage implements AutoCloseable {

  private static final String TAG = "PdfImage";

  static {
    try {
      System.loadLibrary("podofo");
    } catch (UnsatisfiedLinkError e) {
      Log.e(TAG, "Failed to load native library: " + e.getMessage());
      throw e;
    }
  }

  // Package-visible: PdfPainter.drawImage() needs the raw handle.
  long nativeHandle;

  PdfImage(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  public int getWidth() {
    checkOpen();
    return nativeGetWidth(nativeHandle);
  }

  public int getHeight() {
    checkOpen();
    return nativeGetHeight(nativeHandle);
  }

  private void checkOpen() {
    if (nativeHandle == 0) {
      throw new IllegalStateException("Image is closed");
    }
  }

  @Override
  public void close() {
    if (nativeHandle != 0) {
      nativeDestroy(nativeHandle);
      nativeHandle = 0;
    }
  }

  private native int nativeGetWidth(long handle);

  private native int nativeGetHeight(long handle);

  private native void nativeDestroy(long handle);
}
