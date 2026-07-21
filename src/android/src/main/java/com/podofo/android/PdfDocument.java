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
 * drawing/painting, forms, and everything else under
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
   * instances already obtained for later pages become invalid - matches
   * PoDoFo's own C++ semantics (indices shift after removal).
   *
   * @throws PoDoFoException if index is out of range
   */
  public void removePageAt(int index) throws PoDoFoException {
    checkOpen();
    nativeRemovePageAt(nativeHandle, index);
  }

  /**
   * Creates a new page and inserts it at the given 0-based index. Pages
   * at or after {@code index} shift forward by one. The companion to
   * {@link #createPage(double, double)} (which always appends) and
   * {@link #removePageAt(int)} - together these cover page reordering when
   * combined with {@link PdfPage#moveTo(int)}.
   *
   * @throws PoDoFoException if index is out of range or the page couldn't
   *                         be created
   */
  public PdfPage createPageAt(int index, double width, double height) throws PoDoFoException {
    checkOpen();
    long pageHandle = nativeCreatePageAt(nativeHandle, index, width, height);
    if (pageHandle == 0) {
      throw new PoDoFoException("Failed to create page at index " + index);
    }
    return new PdfPage(pageHandle);
  }

  /**
   * Appends every page from {@code source} to the end of this document.
   * The standard merge primitive: load several {@link PdfDocument}s and
   * call this for each one to concatenate them.
   *
   * @throws PoDoFoException if the page copy fails (eg. corrupt source)
   */
  public void appendPagesFrom(PdfDocument source) throws PoDoFoException {
    checkOpen();
    if (source == null) {
      throw new IllegalArgumentException("source must not be null");
    }
    source.checkOpen();
    nativeAppendDocumentPages(nativeHandle, source.nativeHandle);
  }

  /**
   * Appends a contiguous range of pages from {@code source} to the end of
   * this document. This is also the split primitive: create an empty
   * {@link PdfDocument#createNew()}, then call this with the desired
   * {@code pageIndex}/{@code pageCount} from a source document, then
   * {@link #save(String)} the result - the source document isn't modified.
   *
   * @param source    the document to copy pages from
   * @param pageIndex 0-based index of the first page in {@code source} to copy
   * @param pageCount number of pages to copy
   * @throws PoDoFoException if the range is out of bounds or the copy fails
   */
  public void appendPagesFrom(PdfDocument source, int pageIndex, int pageCount) throws PoDoFoException {
    checkOpen();
    if (source == null) {
      throw new IllegalArgumentException("source must not be null");
    }
    source.checkOpen();
    nativeAppendDocumentPagesRange(nativeHandle, source.nativeHandle, pageIndex, pageCount);
  }

  /**
   * Inserts a single page from {@code source} at the given 0-based index in
   * this document. Pages at or after {@code atIndex} shift forward by one.
   *
   * @param atIndex   insertion point in this document
   * @param source    the document to copy a page from
   * @param pageIndex 0-based index of the page in {@code source} to copy
   * @throws PoDoFoException if either index is out of bounds or the copy fails
   */
  public void insertPageFrom(int atIndex, PdfDocument source, int pageIndex) throws PoDoFoException {
    checkOpen();
    if (source == null) {
      throw new IllegalArgumentException("source must not be null");
    }
    source.checkOpen();
    nativeInsertDocumentPageAt(nativeHandle, atIndex, source.nativeHandle, pageIndex);
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
   * Loads a font from a font file (TTF/OTF) for use with
   * {@link PdfPainter#setFont(PdfFont, double)}. This is the counterpart to
   * {@link #getStandard14Font(String)} for apps that want a specific
   * embedded font rather than one of the 14 built-ins — there is no
   * by-name system-font search available (needs Fontconfig, unavailable on
   * Android; see SUMMARIZE.md).
   *
   * @param fontFilePath path to a font file readable from native code (eg.
   *                     a file copied out of the app's assets — a raw
   *                     assets/ path is not directly usable here)
   * @throws PoDoFoException if the font couldn't be loaded
   */
  public PdfFont getOrCreateFont(String fontFilePath) throws PoDoFoException {
    checkOpen();
    if (fontFilePath == null) {
      throw new IllegalArgumentException("fontFilePath must not be null");
    }
    long fontHandle = nativeGetOrCreateFont(nativeHandle, fontFilePath);
    if (fontHandle == 0) {
      throw new PoDoFoException("Failed to load font: " + fontFilePath);
    }
    return new PdfFont(fontHandle);
  }

  /**
   * Loads a font (TTF/OTF) already in memory — the counterpart to
   * {@link #getOrCreateFont(String)} for fonts bundled as app resources
   * (eg. Android {@code assets/}) that would otherwise need to be copied
   * out to a temporary file first just to get a native-readable path.
   *
   * @param data the raw font file bytes
   * @throws PoDoFoException if the font couldn't be loaded
   */
  public PdfFont getOrCreateFontFromBuffer(byte[] data) throws PoDoFoException {
    checkOpen();
    if (data == null) {
      throw new IllegalArgumentException("data must not be null");
    }
    long fontHandle = nativeGetOrCreateFontFromBuffer(nativeHandle, data);
    if (fontHandle == 0) {
      throw new PoDoFoException("Failed to load font from buffer");
    }
    return new PdfFont(fontHandle);
  }

  /**
   * Decodes an encoded image (JPEG/PNG/etc.) from a buffer and embeds it
   * in the document, ready to draw via
   * {@link PdfPainter#drawImage(PdfImage, double, double)}.
   *
   * @throws PoDoFoException if the image couldn't be decoded/embedded
   */
  public PdfImage createImageFromBuffer(byte[] data) throws PoDoFoException {
    checkOpen();
    if (data == null) {
      throw new IllegalArgumentException("data must not be null");
    }
    long imageHandle = nativeCreateImageFromBuffer(nativeHandle, data);
    if (imageHandle == 0) {
      throw new PoDoFoException("Failed to create image from buffer");
    }
    return new PdfImage(imageHandle);
  }

  /**
   * Encrypts the document (AES-256, PDF 2.0 revision 6 — PoDoFo's own
   * default algorithm) with {@link PdfPermission#DEFAULT} permissions.
   * Takes effect on the next {@link #save(String)}.
   *
   * @param userPassword  required to open the document at all; pass ""
   *                      for no open password
   * @param ownerPassword required to change permissions/remove protection
   */
  public void setEncrypted(String userPassword, String ownerPassword) {
    setEncrypted(userPassword, ownerPassword, PdfPermission.DEFAULT);
  }

  /**
   * Encrypts the document (AES-256, PDF 2.0 revision 6 — PoDoFo's own
   * default algorithm) with the given permissions. Takes effect on the
   * next {@link #save(String)}. There is currently no binding for
   * choosing a different algorithm/key length or legacy RC4 encryption.
   *
   * @param userPassword  required to open the document at all; pass ""
   *                      for no open password
   * @param ownerPassword required to change permissions/remove protection
   * @param permissions   {@link PdfPermission} flags OR'd together
   */
  public void setEncrypted(String userPassword, String ownerPassword, int permissions) {
    checkOpen();
    if (userPassword == null || ownerPassword == null) {
      throw new IllegalArgumentException("userPassword/ownerPassword must not be null (use \"\" for none)");
    }
    nativeSetEncrypted(nativeHandle, userPassword, ownerPassword, permissions);
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

  /**
   * @return whether this document has an encryption dictionary
   */
  public boolean isEncrypted() {
    checkOpen();
    return nativeIsEncrypted(nativeHandle);
  }

  /** @return the encryption algorithm name, or {@code null} when unencrypted */
  public String getEncryptionAlgorithm() {
    checkOpen();
    return nativeGetEncryptionAlgorithm(nativeHandle);
  }

  /** @return the encryption key length in bits, or {@code 0} when unencrypted */
  public int getEncryptionKeyLengthBits() {
    checkOpen();
    return nativeGetEncryptionKeyLengthBits(nativeHandle);
  }

  /** @return the PDF security-handler revision, or {@code 0} when unencrypted */
  public int getEncryptionRevision() {
    checkOpen();
    return nativeGetEncryptionRevision(nativeHandle);
  }

  /** @return whether document metadata is encrypted */
  public boolean isMetadataEncrypted() {
    checkOpen();
    return nativeIsMetadataEncrypted(nativeHandle);
  }

  /** @return whether the owner password is configured */
  public boolean isOwnerPasswordSet() {
    checkOpen();
    return nativeIsOwnerPasswordSet(nativeHandle);
  }

  /** @return whether the encryption dictionary was parsed from the PDF */
  public boolean isEncryptionParsed() {
    checkOpen();
    return nativeIsEncryptionParsed(nativeHandle);
  }

  /** @return the raw PDF permissions bitmask, or {@code 0} when unencrypted */
  public int getEncryptionPermissions() {
    checkOpen();
    return nativeGetEncryptionPermissions(nativeHandle);
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

  private native boolean nativeIsEncrypted(long handle);

  private native String nativeGetEncryptionAlgorithm(long handle);

  private native int nativeGetEncryptionKeyLengthBits(long handle);

  private native int nativeGetEncryptionRevision(long handle);

  private native boolean nativeIsMetadataEncrypted(long handle);

  private native boolean nativeIsOwnerPasswordSet(long handle);

  private native boolean nativeIsEncryptionParsed(long handle);

  private native int nativeGetEncryptionPermissions(long handle);

  private native int nativeGetPageCount(long handle);

  private native long nativeGetPage(long handle, int index);

  private native long nativeCreatePage(long handle, double width, double height);

  private native void nativeRemovePageAt(long handle, int index);

  private native long nativeCreatePageAt(long handle, int index, double width, double height);

  private native void nativeAppendDocumentPages(long handle, long sourceHandle);

  private native void nativeAppendDocumentPagesRange(long handle, long sourceHandle, int pageIndex, int pageCount);

  private native void nativeInsertDocumentPageAt(long handle, int atIndex, long sourceHandle, int pageIndex);

  private native long nativeGetStandard14Font(long handle, String name);

  private native int nativeGetFieldCount(long handle);

  private native long nativeGetFieldAt(long handle, int index);

  private native long nativeCreateField(long handle, String name, String fieldType);

  private native long nativeGetOrCreateOutlines(long handle);

  private native long nativeGetOrCreateFont(long handle, String fontFilePath);

  private native long nativeGetOrCreateFontFromBuffer(long handle, byte[] data);

  private native long nativeCreateImageFromBuffer(long handle, byte[] data);

  private native void nativeSetEncrypted(long handle, String userPassword, String ownerPassword, int permissions);

  private native String nativeGetTitle(long handle);

  private native void nativeSetTitle(long handle, String title);

  private native String nativeGetAuthor(long handle);

  private native void nativeSetAuthor(long handle, String author);

  private native String nativeGetSubject(long handle);

  private native void nativeSetSubject(long handle, String subject);

  private native String nativeGetCreator(long handle);

  private native void nativeSetCreator(long handle, String creator);
}
