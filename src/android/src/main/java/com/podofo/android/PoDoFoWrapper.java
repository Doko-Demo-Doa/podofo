package com.podofo.android;

import android.util.Log;

import java.util.List;

/**
 * Java wrapper for the native PoDoFoWrapper library
 */
public class PoDoFoWrapper implements AutoCloseable {

  private static final String TAG = "PoDoFoWrapper";

  // Error domain constant
  public static final String ERROR_DOMAIN = "org.podofo.PodofoSigner";

  // Load the native library
  static {
    try {
      System.loadLibrary("podofo");
    } catch (UnsatisfiedLinkError e) {
      Log.e(TAG, "Failed to load native library: " + e.getMessage());
      throw e;
    }
  }

  // Native handle to the C++ PoDoFoWrapper
  private long nativeHandle;

  private final String conformanceLevel;
  private final String hashAlgorithm;
  private final String inputPath;
  private final String outputPath;
  private final String certificate;
  private final String[] chainCertificates;
  private final String rootCertificate;

  /**
   * Initialize the PDF signer with required parameters
   *
   * @param conformanceLevel  The PDF conformance level
   * @param hashAlgorithm     The hash algorithm to use
   * @param inputPath         Path to the input PDF file
   * @param outputPath        Path to save the signed PDF file
   * @param certificate       The signing certificate, base64-encoded DER (not PEM: no
   *                          "-----BEGIN/END-----" header/footer, no line wrapping)
   * @param chainCertificates Array of chain certificates, each base64-encoded DER (same
   *                          format as {@code certificate})
   * @throws IllegalArgumentException if any of the required parameters are null
   * @throws PoDoFoException          if native initialization fails
   */
  public PoDoFoWrapper(String conformanceLevel, String hashAlgorithm,
      String inputPath, String outputPath, String certificate,
      String[] chainCertificates) throws PoDoFoException {
    this(conformanceLevel, hashAlgorithm, inputPath, outputPath, certificate,
        chainCertificates, null);
  }

  /**
   * Initialize the PDF signer with required parameters
   *
   * @param conformanceLevel  The PDF conformance level
   * @param hashAlgorithm     The hash algorithm to use
   * @param inputPath         Path to the input PDF file
   * @param outputPath        Path to save the signed PDF file
   * @param certificate       The signing certificate, base64-encoded DER (not PEM: no
   *                          "-----BEGIN/END-----" header/footer, no line wrapping)
   * @param chainCertificates Array of chain certificates, each base64-encoded DER (same
   *                          format as {@code certificate})
   * @param rootCertificate   Optional root certificate, base64-encoded DER (same format as
   *                          {@code certificate})
   * @throws IllegalArgumentException if any of the required parameters are null
   * @throws PoDoFoException          if native initialization fails
   */
  public PoDoFoWrapper(String conformanceLevel, String hashAlgorithm,
      String inputPath, String outputPath, String certificate,
      String[] chainCertificates, String rootCertificate) throws PoDoFoException {
    this.conformanceLevel = conformanceLevel;
    this.hashAlgorithm = hashAlgorithm;
    this.inputPath = inputPath;
    this.outputPath = outputPath;
    this.certificate = certificate;
    this.chainCertificates = chainCertificates;
    this.rootCertificate = rootCertificate;

    // Initialize native wrapper
    System.out.println("PoDoFoWrapper: Initializing PoDoFo wrapper");
    System.out.println("PoDoFoWrapper: Conformance Level: " + conformanceLevel);
    System.out.println("PoDoFoWrapper: Hash Algorithm: " + hashAlgorithm);
    System.out.println("PoDoFoWrapper: Input Path: " + inputPath);
    System.out.println("PoDoFoWrapper: Output Path: " + outputPath);
    System.out.println("PoDoFoWrapper: Certificate: " + (certificate != null ? certificate : "null"));
    System.out.println(
        "PoDoFoWrapper: Chain Certificates count: " + (chainCertificates != null ? chainCertificates.length : 0));
    System.out.println("PoDoFoWrapper: Root Certificate: " + (rootCertificate != null ? rootCertificate : "null"));

    System.out.println("PoDoFoWrapper: Calling nativeInit");
    nativeHandle = nativeInit(conformanceLevel, hashAlgorithm, inputPath, outputPath,
        certificate, chainCertificates, rootCertificate);
    System.out.println("PoDoFoWrapper: nativeInit returned handle: " + nativeHandle);

    if (nativeHandle == 0) {
      Log.w(TAG, "Error during initialization: Failed to initialize native PoDoFo wrapper");
      throw new PoDoFoException("Failed to initialize native PoDoFo wrapper");
    }
  }

  /**
   * Check if the native library is loaded and session is initialized
   *
   * @return true if the library is loaded and session is initialized, false
   *         otherwise
   */
  public boolean isLoaded() {
    return nativeIsLoaded(nativeHandle);
  }

  /**
   * Print the current state of the session (for debugging purposes)
   */
  public void printState() {
    if (nativeHandle != 0) {
      nativePrintState(nativeHandle);
    }
  }

  /**
   * Sets the signature /Name entry.
   * Call before {@link #calculateHash()}.
   */
  public void setSignerName(String name) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetSignerName(nativeHandle, name);
  }

  /**
   * Sets the signature /Prop_Build/App/Name entry.
   * Call before {@link #calculateHash()}.
   */
  public void setCreatingApplication(String application) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetCreatingApplication(nativeHandle, application);
  }

  /**
   * Sets the signature /Location entry.
   * Call before {@link #calculateHash()}.
   */
  public void setSignatureLocation(String location) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetSignatureLocation(nativeHandle, location);
  }

  /**
   * Sets the signature /Reason entry.
   * Call before {@link #calculateHash()}.
   */
  public void setSignatureReason(String reason) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetSignatureReason(nativeHandle, reason);
  }

  /**
   * Sets the signature /ContactInfo entry.
   * Call before {@link #calculateHash()}.
   */
  public void setSignatureContactInfo(String contactInfo) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetSignatureContactInfo(nativeHandle, contactInfo);
  }

  /**
   * Makes the signature visible by placing its widget on a page.
   * Call before {@link #calculateHash()}.
   */
  public void setVisibleSignature(int pageIndex, double x, double y, double width, double height, String text,
      String fontName)
      throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeSetVisibleSignature(nativeHandle, pageIndex, x, y, width, height, text, fontName);
  }

  /**
   * Calculate hash for signing
   *
   * @return The hash as a string, or null if calculation failed
   * @throws PoDoFoException if there is an error during hash calculation
   */
  public String calculateHash() throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeCalculateHash(nativeHandle);
  }

  /**
   * Finalize the signing process with the provided signed hash
   *
   * @param signedHash   The signed hash to use for finalizing
   * @param tsr          The timestamp service response
   * @param certificates An array of base64-encoded certificates for the DSS
   *                     dictionary
   * @param crls         An array of base64-encoded CRLs for the DSS dictionary
   * @param ocsps        An array of base64-encoded OCSP responses for the DSS
   *                     dictionary
   * @throws PoDoFoException if there is an error during finalization
   */
  public void finalizeSigningWithSignedHash(String signedHash, String tsr,
      List<String> certificates, List<String> crls, List<String> ocsps) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    if (signedHash == null) {
      throw new PoDoFoException("Cannot finalize with nil signed hash");
    }
    nativeFinalizeSigningWithSignedHash(nativeHandle, signedHash, tsr, certificates, crls, ocsps);
  }

  /**
   * Begins the LTA (Long-Term Archive) signature process.
   * This should be called after a B-LT signature has been created.
   *
   * @return The hash to be sent to the Timestamping Authority.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String beginSigningLTA() throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeBeginSigningLTA(nativeHandle);
  }

  /**
   * Finalizes the LTA signature process with a timestamp response.
   *
   * @param tsr          The timestamp service response (base64 encoded).
   * @param certificates An array of base64-encoded certificates for the DSS
   *                     dictionary.
   * @param crls         An array of base64-encoded CRLs for the DSS dictionary.
   * @param ocsps        An array of base64-encoded OCSP responses for the DSS
   *                     dictionary.
   * @throws PoDoFoException if there is an error during the process.
   */
  public void finishSigningLTA(String tsr, List<String> certificates, List<String> crls, List<String> ocsps)
      throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    nativeFinishSigningLTA(nativeHandle, tsr, certificates, crls, ocsps);
  }

  /**
   * Extracts the CRL distribution point URL from a base64 encoded certificate.
   *
   * @param base64Cert The base64 encoded certificate.
   * @return The CRL URL as a string.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String getCrlFromCertificate(String base64Cert) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeGetCrlFromCertificate(nativeHandle, base64Cert);
  }

  /**
   * Extracts the TSA signer certificate from a base64-encoded TSR.
   *
   * @param base64Tsr The base64-encoded TSR (timestamp response).
   * @return The base64 DER encoding of the signer certificate.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String extractSignerCertFromTSR(String base64Tsr) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeExtractSignerCertFromTSR(nativeHandle, base64Tsr);
  }

  /**
   * Extracts the TSA issuer certificate from a base64-encoded TSR.
   *
   * @param base64Tsr The base64-encoded TSR (timestamp response).
   * @return The base64 DER encoding of the issuer certificate.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String extractIssuerCertFromTSR(String base64Tsr) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeExtractIssuerCertFromTSR(nativeHandle, base64Tsr);
  }

  /**
   * Extracts the OCSP responder URL from a certificate's AIA extension.
   *
   * @param base64Cert       The certificate encoded in base64.
   * @param base64IssuerCert The issuer certificate encoded in base64.
   * @return The OCSP responder URL as a string.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String getOCSPFromCertificate(String base64Cert, String base64IssuerCert) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeGetOCSPFromCertificate(nativeHandle, base64Cert, base64IssuerCert);
  }

  /**
   * Gets an OCSP request from base64-encoded certificates and returns it as
   * base64.
   *
   * @param base64Cert       The certificate encoded in base64.
   * @param base64IssuerCert The issuer certificate encoded in base64.
   * @return The base64-encoded OCSP request.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String buildOCSPRequestFromCertificates(String base64Cert, String base64IssuerCert) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeBuildOCSPRequestFromCertificates(nativeHandle, base64Cert, base64IssuerCert);
  }

  /**
   * Extracts the CA Issuers URL from a certificate's AIA extension.
   *
   * @param base64Cert The certificate encoded in base64.
   * @return The CA Issuers URL as a string.
   * @throws PoDoFoException if there is an error during the process.
   */
  public String getCertificateIssuerUrlFromCertificate(String base64Cert) throws PoDoFoException {
    if (nativeHandle == 0) {
      throw new PoDoFoException("Session not initialized");
    }
    return nativeGetCertificateIssuerUrlFromCertificate(nativeHandle, base64Cert);
  }

  // Native methods implemented in C++
  private native long nativeInit(String conformanceLevel, String hashAlgorithm,
      String inputPath, String outputPath,
      String certificate, String[] chainCertificates, String rootCertificate);

  private native boolean nativeIsLoaded(long handle);

  private native void nativePrintState(long handle);

  private native void nativeSetSignerName(long handle, String name);

  private native void nativeSetCreatingApplication(long handle, String application);

  private native void nativeSetSignatureLocation(long handle, String location);

  private native void nativeSetSignatureReason(long handle, String reason);

  private native void nativeSetSignatureContactInfo(long handle, String contactInfo);

  private native void nativeSetVisibleSignature(long handle, int pageIndex,
      double x, double y, double width, double height, String text, String fontName);

  private native String nativeCalculateHash(long handle);

  private native void nativeFinalizeSigningWithSignedHash(long handle, String signedHash, String tsr,
      List<String> certificates, List<String> crls, List<String> ocsps);

  private native String nativeBeginSigningLTA(long handle);

  private native void nativeFinishSigningLTA(long handle, String tsr, List<String> certificates, List<String> crls,
      List<String> ocsps);

  private native String nativeGetCrlFromCertificate(long handle, String base64Cert);

  private native String nativeExtractSignerCertFromTSR(long handle, String base64Tsr);

  private native String nativeExtractIssuerCertFromTSR(long handle, String base64Tsr);

  private native String nativeGetOCSPFromCertificate(long handle, String base64Cert, String base64IssuerCert);

  private native String nativeBuildOCSPRequestFromCertificates(long handle, String base64Cert, String base64IssuerCert);

  private native String nativeGetCertificateIssuerUrlFromCertificate(long handle, String base64Cert);

  /**
   * Clean up native resources
   */
  @Override
  public void close() {
    if (nativeHandle != 0) {
      nativeCleanup(nativeHandle);
      nativeHandle = 0;
    }
  }

  private native void nativeCleanup(long handle);
}
