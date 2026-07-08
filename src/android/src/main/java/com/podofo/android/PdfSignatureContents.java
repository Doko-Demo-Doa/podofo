package com.podofo.android;

import java.util.Collections;
import java.util.List;

/**
 * Parsed CMS/PKCS7 contents of a PDF digital signature.
 *
 * <p>Created by {@link PdfSignature#parseContents()}.
 */
public class PdfSignatureContents {

  private final List<SignerInfo> signerInfos;
  private final TimestampInfo timestampToken;

  PdfSignatureContents(List<SignerInfo> signerInfos, TimestampInfo timestampToken) {
    this.signerInfos = Collections.unmodifiableList(signerInfos);
    this.timestampToken = timestampToken;
  }

  /** @return signer information entries found in the CMS message */
  public List<SignerInfo> getSignerInfos() {
    return signerInfos;
  }

  /** @return the embedded RFC3161 timestamp token, or null if absent */
  public TimestampInfo getTimestampToken() {
    return timestampToken;
  }

  /** Information about a CMS signer. */
  public static class SignerInfo {
    /** DER-encoded signer certificate. */
    public final byte[] certificate;
    /** X.509 subject name (RFC 2253 style). */
    public final String subject;
    /** X.509 issuer name (RFC 2253 style). */
    public final String issuer;
    /** Certificate serial number as hex. */
    public final String serial;
    /**
     * CMS authenticated signingTime attribute in W3C format, or null.
     * Note: PoDoFo's default PAdES-B signer intentionally omits this attribute.
     */
    public final String signingTime;

    SignerInfo(byte[] certificate, String subject, String issuer, String serial, String signingTime) {
      this.certificate = certificate;
      this.subject = subject;
      this.issuer = issuer;
      this.serial = serial;
      this.signingTime = signingTime;
    }
  }

  /** Information about an embedded RFC3161 timestamp token. */
  public static class TimestampInfo {
    /** genTime from the TSTInfo in W3C format, or null. */
    public final String genTime;
    /** DER-encoded TSA certificate, if embedded in the token. */
    public final byte[] tsaCertificate;
    /** TSA subject name, if available. */
    public final String tsaSubject;

    TimestampInfo(String genTime, byte[] tsaCertificate, String tsaSubject) {
      this.genTime = genTime;
      this.tsaCertificate = tsaCertificate;
      this.tsaSubject = tsaSubject;
    }
  }
}
