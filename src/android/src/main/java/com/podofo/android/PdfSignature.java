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
   * <p>It performs no certificate trust validation, and it doesn't inspect
   * the document beyond the signed revision: a complete verification is a
   * document wide concern.
   *
   * @param documentPath path to the (signed) PDF file this signature came from
   */
  public VerifyStatus verifySignature(String documentPath) {
    if (documentPath == null) {
      throw new IllegalArgumentException("documentPath must not be null");
    }
    return VerifyStatus.fromNative(nativeVerifySignatureFromPath(nativeHandle, documentPath));
  }

  /** Result of {@link #verifySignature(String)}. */
  public enum VerifyStatus {
    /**
     * The signature could not be checked. Missing or invalid PKCS7/CMS
     * contents, unsupported signer info layout, or a malformed /ByteRange.
     */
    INDETERMINATE,

    /**
     * The signature is cryptographically invalid: the signed bytes were
     * modified, or the signature doesn't correspond to the certificate.
     */
    INVALID,

    /**
     * The CMS signature is cryptographically valid over the signed bytes,
     * but the /ByteRange doesn't reach the end of the input: the document
     * has content that is not covered by the signature. The certificate
     * trust status is indeterminate.
     */
    CRYPTO_VERIFIED_PARTIAL_COVERAGE,

    /**
     * The CMS signature is cryptographically valid and the /ByteRange
     * covers the whole input. The certificate trust status is indeterminate.
     */
    CRYPTO_VERIFIED;

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
  private native int nativeVerifySignatureFromPath(long handle, String documentPath);
}
