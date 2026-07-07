package com.podofo.android;

import android.util.Log;

/**
 * Draws text and simple shapes onto a {@link PdfPage} — wraps PoDoFo's
 * {@code PdfPainter}. Only a basic slice of the C++ API is bound so far:
 * lines/rectangles/circles, RGB stroking/non-stroking color, and text
 * drawn with a {@link PdfFont} (currently only the 14 standard fonts via
 * {@link PdfDocument#getStandard14Font(String)} — no custom/bundled font
 * loading yet). See SUMMARIZE.md for what's not covered (paths, images,
 * clipping, non-RGB color spaces, patterns, text alignment/wrapping).
 *
 * Usage:
 * <pre>{@code
 * try (PdfPainter painter = new PdfPainter(page)) {
 *   painter.setFont(font, 12);
 *   painter.drawText("Hello", 50, 700);
 *   painter.finishDrawing();
 * }
 * }</pre>
 *
 * {@link #finishDrawing()} must be called before {@link #close()} —
 * matches PoDoFo's own contract that a painting session has to be
 * explicitly finished before the page's content stream is valid.
 */
public class PdfPainter implements AutoCloseable {

  private static final String TAG = "PdfPainter";

  static {
    try {
      System.loadLibrary("podofo");
    } catch (UnsatisfiedLinkError e) {
      Log.e(TAG, "Failed to load native library: " + e.getMessage());
      throw e;
    }
  }

  private long nativeHandle;

  /**
   * Creates a painter bound to the given page, ready to draw.
   *
   * @throws PoDoFoException if native initialization fails
   */
  public PdfPainter(PdfPage page) throws PoDoFoException {
    if (page == null) {
      throw new IllegalArgumentException("page must not be null");
    }
    nativeHandle = nativeCreate();
    if (nativeHandle == 0) {
      throw new PoDoFoException("Failed to create native PdfPainter");
    }
    try {
      nativeSetCanvas(nativeHandle, page.nativeHandle);
    } catch (RuntimeException e) {
      nativeDestroy(nativeHandle);
      nativeHandle = 0;
      throw e;
    }
  }

  /**
   * Sets the font and size used by subsequent {@link #drawText(String, double, double)}
   * calls. Must be called at least once before drawing any text.
   */
  public void setFont(PdfFont font, double fontSize) {
    checkOpen();
    if (font == null) {
      throw new IllegalArgumentException("font must not be null");
    }
    nativeSetFont(nativeHandle, font.nativeHandle, fontSize);
  }

  /**
   * Draws a single line of text. {@link #setFont(PdfFont, double)} must be
   * called first.
   */
  public void drawText(String text, double x, double y) {
    checkOpen();
    if (text == null) {
      throw new IllegalArgumentException("text must not be null");
    }
    nativeDrawText(nativeHandle, text, x, y);
  }

  /** Strokes a line with the current stroking color and line settings. */
  public void drawLine(double x1, double y1, double x2, double y2) {
    checkOpen();
    nativeDrawLine(nativeHandle, x1, y1, x2, y2);
  }

  /**
   * Draws a rectangle.
   *
   * @param fill true to fill using the non-zero winding rule, false to
   *             stroke the outline only
   */
  public void drawRectangle(double x, double y, double width, double height, boolean fill) {
    checkOpen();
    nativeDrawRectangle(nativeHandle, x, y, width, height, fill);
  }

  /**
   * Draws a circle.
   *
   * @param fill true to fill using the non-zero winding rule, false to
   *             stroke the outline only
   */
  public void drawCircle(double x, double y, double radius, boolean fill) {
    checkOpen();
    nativeDrawCircle(nativeHandle, x, y, radius, fill);
  }

  /** Sets the color used for stroking (line/outline) operations. */
  public void setStrokingColorRGB(double red, double green, double blue) {
    checkOpen();
    nativeSetStrokingColorRGB(nativeHandle, red, green, blue);
  }

  /** Sets the color used for non-stroking (fill/text) operations. */
  public void setNonStrokingColorRGB(double red, double green, double blue) {
    checkOpen();
    nativeSetNonStrokingColorRGB(nativeHandle, red, green, blue);
  }

  /** Pushes the current graphics state (matches the PDF 'q' operator). */
  public void save() {
    checkOpen();
    nativeSave(nativeHandle);
  }

  /** Pops the graphics state (matches the PDF 'Q' operator). */
  public void restore() {
    checkOpen();
    nativeRestore(nativeHandle);
  }

  /**
   * Finishes drawing onto the page. Must be called once drawing is
   * complete, before {@link #close()}.
   */
  public void finishDrawing() {
    checkOpen();
    nativeFinishDrawing(nativeHandle);
  }

  private void checkOpen() {
    if (nativeHandle == 0) {
      throw new IllegalStateException("Painter is closed");
    }
  }

  @Override
  public void close() {
    if (nativeHandle != 0) {
      nativeDestroy(nativeHandle);
      nativeHandle = 0;
    }
  }

  private static native long nativeCreate();

  private native void nativeSetCanvas(long handle, long pageHandle);

  private native void nativeDestroy(long handle);

  private native void nativeFinishDrawing(long handle);

  private native void nativeSetFont(long handle, long fontHandle, double fontSize);

  private native void nativeDrawText(long handle, String text, double x, double y);

  private native void nativeDrawLine(long handle, double x1, double y1, double x2, double y2);

  private native void nativeDrawRectangle(long handle, double x, double y, double width, double height, boolean fill);

  private native void nativeDrawCircle(long handle, double x, double y, double radius, boolean fill);

  private native void nativeSetStrokingColorRGB(long handle, double red, double green, double blue);

  private native void nativeSetNonStrokingColorRGB(long handle, double red, double green, double blue);

  private native void nativeSave(long handle);

  private native void nativeRestore(long handle);
}
