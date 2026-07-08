package com.podofo.android;

/**
 * A node in the document's outline (bookmark) tree — wraps PoDoFo's
 * {@code PdfOutlineItem}/{@code PdfOutlines} (the root returned by
 * {@link PdfDocument#getOrCreateOutlines()} is itself a {@code PdfOutlines},
 * which extends {@code PdfOutlineItem} on the C++ side, so one Java class
 * covers both).
 *
 * Owned by the document's outline tree (same as {@link PdfPage}) — no
 * public constructor, no close().
 */
public class PdfOutlineItem {

  final long nativeHandle;

  PdfOutlineItem(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  public String getTitle() {
    return nativeGetTitle(nativeHandle);
  }

  public void setTitle(String title) {
    if (title == null) {
      throw new IllegalArgumentException("title must not be null");
    }
    nativeSetTitle(nativeHandle, title);
  }

  /**
   * Creates a new child item, becoming this item's first child (or
   * appended after existing children).
   */
  public PdfOutlineItem createChild(String title) throws PoDoFoException {
    if (title == null) {
      throw new IllegalArgumentException("title must not be null");
    }
    long handle = nativeCreateChild(nativeHandle, title);
    if (handle == 0) {
      throw new PoDoFoException("Failed to create child outline item");
    }
    return new PdfOutlineItem(handle);
  }

  /**
   * Creates a new item on the same level, immediately following this one.
   */
  public PdfOutlineItem createNext(String title) throws PoDoFoException {
    if (title == null) {
      throw new IllegalArgumentException("title must not be null");
    }
    long handle = nativeCreateNext(nativeHandle, title);
    if (handle == 0) {
      throw new PoDoFoException("Failed to create next outline item");
    }
    return new PdfOutlineItem(handle);
  }

  /** @return the first child item, or null if this item has no children */
  public PdfOutlineItem getFirst() {
    long handle = nativeGetFirst(nativeHandle);
    return handle == 0 ? null : new PdfOutlineItem(handle);
  }

  /** @return the next sibling item, or null if this is the last on its level */
  public PdfOutlineItem getNext() {
    long handle = nativeGetNext(nativeHandle);
    return handle == 0 ? null : new PdfOutlineItem(handle);
  }

  /** @return the parent item, or null if this is the top-level outlines root */
  public PdfOutlineItem getParent() {
    long handle = nativeGetParent(nativeHandle);
    return handle == 0 ? null : new PdfOutlineItem(handle);
  }

  /**
   * Points this item at a page, with the page scaled to fit the viewer
   * window (the common "jump to page" case). For finer-grained destinations
   * (specific rect/zoom) there is currently no binding.
   */
  public void setDestinationToPage(PdfPage page) {
    if (page == null) {
      throw new IllegalArgumentException("page must not be null");
    }
    nativeSetDestinationToPage(nativeHandle, page.nativeHandle);
  }

  private native String nativeGetTitle(long handle);

  private native void nativeSetTitle(long handle, String title);

  private native long nativeCreateChild(long handle, String title);

  private native long nativeCreateNext(long handle, String title);

  private native long nativeGetFirst(long handle);

  private native long nativeGetNext(long handle);

  private native long nativeGetParent(long handle);

  private native void nativeSetDestinationToPage(long handle, long pageHandle);
}
