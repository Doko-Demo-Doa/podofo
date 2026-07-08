package com.podofo.android;

/**
 * A signature AcroForm field — wraps PoDoFo's {@code PdfSignature}.
 *
 * <p>Obtain an instance from {@link PdfField#asSignature()}.
 *
 * <p>The handle is non-owning: the underlying {@code PdfSignature} is owned by
 * the document's AcroForm, just like {@link PdfField}.
 */
public class PdfSignature {

  private final long nativeHandle;

  PdfSignature(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  /** @return true if the field has a signature value object with /Contents set */
  public boolean hasSignatureValue() {
    return nativeHasSignatureValue(nativeHandle);
  }

  /** @return the signature handler filter, e.g. "Adobe.PPKLite", or null */
  public String getFilter() {
    return nativeGetFilter(nativeHandle);
  }

  /** @return the signature sub-filter, e.g. "ETSI.CAdES.detached", or null */
  public String getSubFilter() {
    return nativeGetSubFilter(nativeHandle);
  }

  /** @return the signature type, e.g. "Sig" or "DocTimeStamp", or null */
  public String getType() {
    return nativeGetType(nativeHandle);
  }

  /** @return the /Name entry (signer name as declared by the signing tool), or null */
  public String getName() {
    return nativeGetName(nativeHandle);
  }

  /** @return the /Reason entry, or null */
  public String getReason() {
    return nativeGetReason(nativeHandle);
  }

  /** @return the /Location entry, or null */
  public String getLocation() {
    return nativeGetLocation(nativeHandle);
  }

  /** @return the /ContactInfo entry, or null */
  public String getContactInfo() {
    return nativeGetContactInfo(nativeHandle);
  }

  /**
   * @return the /M entry (claimed signing date) in W3C format, or null.
   * This is the date declared in the PDF dictionary, not a cryptographically
   * attested CMS signingTime.
   */
  public String getSignDate() {
    return nativeGetSignDate(nativeHandle);
  }

  /** @return the /ByteRange array, or null */
  public long[] getByteRange() {
    return nativeGetByteRange(nativeHandle);
  }

  /** @return the raw /Prop_Build dictionary as a PDF string, or null */
  public String getPropBuild() {
    return nativeGetPropBuild(nativeHandle);
  }

  /** @return the raw CMS/PKCS7 bytes from /Contents, or null */
  public byte[] getContents() {
    return nativeGetContents(nativeHandle);
  }

  /**
   * Convenience: decode /Contents and parse the CMS/PKCS7 blob.
   *
   * @return a {@link PdfSignatureContents} with signer info and optional
   * embedded timestamp token, or null if the signature has no /Contents.
   */
  public PdfSignatureContents parseContents() {
    return nativeParseContents(nativeHandle);
  }

  /**
   * Cryptographically verifies this signature against the actual bytes of
   * the PDF file it belongs to (reads /ByteRange from the file at
   * {@code documentPath} directly — no need to extract the bytes yourself).
   *
   * <p><b>Does NOT validate the signer's certificate chain of trust</b> — no
   * trust store or revocation checking is performed. A {@link VerifyStatus#VALID_NO_TRUST}
   * result means the signature is cryptographically self-consistent (the
   * document wasn't tampered with and the signature matches the embedded
   * certificate), not that the certificate should be trusted.
   *
   * @param documentPath path to the (signed) PDF file this signature came from
   */
  public VerifyStatus verifySignature(String documentPath) {
    if (documentPath == null) {
      throw new IllegalArgumentException("documentPath must not be null");
    }
    return VerifyStatus.fromNative(nativeVerifySignatureFromPath(nativeHandle, documentPath));
  }

  /**
   * Cryptographically verifies this signature against caller-supplied
   * bytes — the exact document content covered by {@link #getByteRange()},
   * with the /Contents entry itself excluded. Use
   * {@link #verifySignature(String)} instead unless you already have these
   * exact bytes for another reason.
   *
   * <p>Same trust caveat as {@link #verifySignature(String)}: does NOT
   * validate the signer's certificate chain of trust.
   */
  public VerifyStatus verifySignature(byte[] signedData) {
    if (signedData == null) {
      throw new IllegalArgumentException("signedData must not be null");
    }
    return VerifyStatus.fromNative(nativeVerifySignature(nativeHandle, signedData));
  }

  /** Result of {@link #verifySignature(byte[])}/{@link #verifySignature(String)}. */
  public enum VerifyStatus {
    /**
     * The signature could not be checked at all (missing /ByteRange,
     * malformed CMS/PKCS7 data, I/O error reading the document, etc.).
     * Distinct from {@link #INVALID} — this means "unable to tell", not
     * "checked, and it's wrong".
     */
    COULD_NOT_VERIFY,

    /**
     * The signature does not match the given bytes, or is otherwise
     * cryptographically invalid (eg. the document was modified after
     * signing).
     */
    INVALID,

    /**
     * The signature is cryptographically valid over the given bytes.
     *
     * <p>This does NOT mean the signer's certificate should be trusted: no
     * certificate chain or revocation checking is performed.
     */
    VALID_NO_TRUST;

    private static VerifyStatus fromNative(int value) {
      return values()[value];
    }
  }

  private native boolean nativeHasSignatureValue(long handle);
  private native String nativeGetFilter(long handle);
  private native String nativeGetSubFilter(long handle);
  private native String nativeGetType(long handle);
  private native String nativeGetName(long handle);
  private native String nativeGetReason(long handle);
  private native String nativeGetLocation(long handle);
  private native String nativeGetContactInfo(long handle);
  private native String nativeGetSignDate(long handle);
  private native long[] nativeGetByteRange(long handle);
  private native String nativeGetPropBuild(long handle);
  private native byte[] nativeGetContents(long handle);
  private native PdfSignatureContents nativeParseContents(long handle);
  private native int nativeVerifySignature(long handle, byte[] signedData);
  private native int nativeVerifySignatureFromPath(long handle, String documentPath);
}
