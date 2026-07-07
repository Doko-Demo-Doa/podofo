package com.podofo.android;

import android.util.Log;

/**
 * Java wrapper for PoDoFo's {@code PdfMemDocument} — general-purpose PDF
 * document I/O and page access (create/load/save, page count, page
 * access/insertion/removal, basic metadata). This is a separate, narrower
 * binding from {@link PoDoFoWrapper} (which wraps the remote-signing
 * session specifically); the two don't interact.
 *
 * Only a slice of PoDoFo's document API is exposed here so far — see
 * SUMMARIZE.md for what's intentionally not bound yet (text extraction,
 * drawing/painting, forms, encryption, and everything else under
 * src/podofo/main/ beyond PdfMemDocument/PdfPageCollection/PdfMetadata).
 */
public class PdfDocument implements AutoCloseable {

  private static final String TAG = "PdfDocument";

  static {
    try {
      System.loadLibrary("podofo");
    } catch (UnsatisfiedLinkError e) {
      Log.e(TAG, "Failed to load native library: " + e.getMessage());
      throw e;
    }
  }

  private long nativeHandle;

  private PdfDocument(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  /**
   * Creates a new, empty PDF document.
   */
  public static PdfDocument createNew() {
    return new PdfDocument(nativeCreate());
  }

  /**
   * Loads a PDF document from a file path.
   *
   * @throws PoDoFoException if the file can't be parsed/opened
   */
  public static PdfDocument load(String path) throws PoDoFoException {
    return load(path, null);
  }

  /**
   * Loads a PDF document from a file path.
   *
   * @param password optional password for encrypted documents
   * @throws PoDoFoException if the file can't be parsed/opened
   */
  public static PdfDocument load(String path, String password) throws PoDoFoException {
    if (path == null) {
      throw new IllegalArgumentException("path must not be null");
    }
    long handle = nativeLoad(path, password);
    if (handle == 0) {
      throw new PoDoFoException("Failed to load PDF document: " + path);
    }
    return new PdfDocument(handle);
  }

  /**
   * Saves the complete document to a file.
   *
   * @throws PoDoFoException if the document can't be written
   */
  public void save(String path) throws PoDoFoException {
    checkOpen();
    if (path == null) {
      throw new IllegalArgumentException("path must not be null");
    }
    nativeSave(nativeHandle, path);
  }

  /**
   * @return the number of pages in this document
   */
  public int getPageCount() {
    checkOpen();
    return nativeGetPageCount(nativeHandle);
  }

  /**
   * Returns the page at the given 0-based index. The returned {@link PdfPage}
   * is owned by this document's page tree, same as in the underlying C++
   * API — it becomes invalid once this document is closed.
   *
   * @throws PoDoFoException if index is out of range
   */
  public PdfPage getPage(int index) throws PoDoFoException {
    checkOpen();
    long pageHandle = nativeGetPage(nativeHandle, index);
    if (pageHandle == 0) {
      throw new PoDoFoException("Failed to get page at index " + index);
    }
    return new PdfPage(pageHandle);
  }

  /**
   * Creates a new page and appends it to the end of the document.
   *
   * @param width  page width in PDF units
   * @param height page height in PDF units
   * @throws PoDoFoException if the page couldn't be created
   */
  public PdfPage createPage(double width, double height) throws PoDoFoException {
    checkOpen();
    long pageHandle = nativeCreatePage(nativeHandle, width, height);
    if (pageHandle == 0) {
      throw new PoDoFoException("Failed to create page");
    }
    return new PdfPage(pageHandle);
  }

  /**
   * Removes the page at the given 0-based index. Any {@link PdfPage}
   * instances already obtained for later pages become invalid — matches
   * PoDoFo's own C++ semantics (indices shift after removal).
   *
   * @throws PoDoFoException if index is out of range
   */
  public void removePageAt(int index) throws PoDoFoException {
    checkOpen();
    nativeRemovePageAt(nativeHandle, index);
  }

  /**
   * Gets one of the 14 PDF "standard" fonts (Helvetica, Times, Courier,
   * Symbol, ZapfDingbats and their bold/italic variants) — always
   * renderable by any PDF viewer, no font embedding needed. This works
   * without Fontconfig (unavailable on Android; see SUMMARIZE.md), unlike
   * PoDoFo's by-name {@code SearchFont()}. There is currently no binding
   * for loading a custom/bundled font file.
   *
   * @param name one of: TimesRoman, TimesItalic, TimesBold, TimesBoldItalic,
   *             Helvetica, HelveticaOblique, HelveticaBold,
   *             HelveticaBoldOblique, Courier, CourierOblique, CourierBold,
   *             CourierBoldOblique, Symbol, ZapfDingbats (matches PoDoFo's
   *             own PdfStandard14FontType names)
   * @throws PoDoFoException if name isn't one of the above
   */
  public PdfFont getStandard14Font(String name) throws PoDoFoException {
    checkOpen();
    if (name == null) {
      throw new IllegalArgumentException("name must not be null");
    }
    long fontHandle = nativeGetStandard14Font(nativeHandle, name);
    if (fontHandle == 0) {
      throw new PoDoFoException("Unknown standard font: " + name);
    }
    return new PdfFont(fontHandle);
  }

  /**
   * @return the number of AcroForm fields in this document (0 if there is
   *         no AcroForm yet)
   */
  public int getFieldCount() {
    checkOpen();
    return nativeGetFieldCount(nativeHandle);
  }

  /**
   * Returns the AcroForm field at the given 0-based index. Owned by the
   * document's AcroForm, same lifetime hazard as {@link #getPage(int)}.
   *
   * @throws PoDoFoException if index is out of range
   */
  public PdfField getFieldAt(int index) throws PoDoFoException {
    checkOpen();
    long fieldHandle = nativeGetFieldAt(nativeHandle, index);
    if (fieldHandle == 0) {
      throw new PoDoFoException("Failed to get field at index " + index);
    }
    return new PdfField(fieldHandle);
  }

  /**
   * Creates a new text box field, creating the document's AcroForm first
   * if it doesn't exist yet.
   *
   * @throws PoDoFoException if the field couldn't be created
   */
  public PdfField createTextBox(String name) throws PoDoFoException {
    checkOpen();
    if (name == null) {
      throw new IllegalArgumentException("name must not be null");
    }
    long fieldHandle = nativeCreateField(nativeHandle, name, "TextBox");
    if (fieldHandle == 0) {
      throw new PoDoFoException("Failed to create text box field: " + name);
    }
    return new PdfField(fieldHandle);
  }

  /**
   * Creates a new checkbox field, creating the document's AcroForm first
   * if it doesn't exist yet.
   *
   * @throws PoDoFoException if the field couldn't be created
   */
  public PdfField createCheckBox(String name) throws PoDoFoException {
    checkOpen();
    if (name == null) {
      throw new IllegalArgumentException("name must not be null");
    }
    long fieldHandle = nativeCreateField(nativeHandle, name, "CheckBox");
    if (fieldHandle == 0) {
      throw new PoDoFoException("Failed to create checkbox field: " + name);
    }
    return new PdfField(fieldHandle);
  }

  /**
   * @return the root of the document's outline (bookmark) tree, creating
   *         it if it doesn't exist yet
   */
  public PdfOutlineItem getOrCreateOutlines() {
    checkOpen();
    return new PdfOutlineItem(nativeGetOrCreateOutlines(nativeHandle));
  }

  public String getTitle() {
    checkOpen();
    return nativeGetTitle(nativeHandle);
  }

  public void setTitle(String title) {
    checkOpen();
    nativeSetTitle(nativeHandle, title);
  }

  public String getAuthor() {
    checkOpen();
    return nativeGetAuthor(nativeHandle);
  }

  public void setAuthor(String author) {
    checkOpen();
    nativeSetAuthor(nativeHandle, author);
  }

  public String getSubject() {
    checkOpen();
    return nativeGetSubject(nativeHandle);
  }

  public void setSubject(String subject) {
    checkOpen();
    nativeSetSubject(nativeHandle, subject);
  }

  public String getCreator() {
    checkOpen();
    return nativeGetCreator(nativeHandle);
  }

  public void setCreator(String creator) {
    checkOpen();
    nativeSetCreator(nativeHandle, creator);
  }

  private void checkOpen() {
    if (nativeHandle == 0) {
      throw new IllegalStateException("Document is closed");
    }
  }

  /**
   * Frees the native document. Any {@link PdfPage} instances obtained from
   * this document become invalid after this call.
   */
  @Override
  public void close() {
    if (nativeHandle != 0) {
      nativeClose(nativeHandle);
      nativeHandle = 0;
    }
  }

  private static native long nativeCreate();

  private static native long nativeLoad(String path, String password);

  private native void nativeSave(long handle, String path);

  private native void nativeClose(long handle);

  private native int nativeGetPageCount(long handle);

  private native long nativeGetPage(long handle, int index);

  private native long nativeCreatePage(long handle, double width, double height);

  private native void nativeRemovePageAt(long handle, int index);

  private native long nativeGetStandard14Font(long handle, String name);

  private native int nativeGetFieldCount(long handle);

  private native long nativeGetFieldAt(long handle, int index);

  private native long nativeCreateField(long handle, String name, String fieldType);

  private native long nativeGetOrCreateOutlines(long handle);

  private native String nativeGetTitle(long handle);

  private native void nativeSetTitle(long handle, String title);

  private native String nativeGetAuthor(long handle);

  private native void nativeSetAuthor(long handle, String author);

  private native String nativeGetSubject(long handle);

  private native void nativeSetSubject(long handle, String subject);

  private native String nativeGetCreator(long handle);

  private native void nativeSetCreator(long handle, String creator);
}
