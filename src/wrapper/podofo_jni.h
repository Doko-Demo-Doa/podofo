#ifndef PODOFO_JNI_H
#define PODOFO_JNI_H

#include <jni.h>
#include <podofo/podofo.h>
#include <string>
#include <vector>
#include <memory>

// Class to manage the PoDoFo session
class PoDoFoWrapper
{
private:
    std::unique_ptr<PoDoFo::PdfRemoteSignDocumentSession> nativeSession;

public:
    // Constructor - made public so std::make_unique can access it
    PoDoFoWrapper() = default;
    ~PoDoFoWrapper() = default;

    // Static factory method
    static std::unique_ptr<PoDoFoWrapper> initialize(const std::string &conformanceLevel,
                                                     const std::string &hashAlgorithm,
                                                     const std::string &inputPath,
                                                     const std::string &outputPath,
                                                     const std::string &certificate,
                                                     const std::vector<std::string> &chainCertificates);

    bool isLoaded() const;
    void printState() const;
    std::string calculateHash();
    void finalizeSigningWithSignedHash(const std::string &signedHash, const std::string &tsr, const std::optional<PoDoFo::ValidationData> &validationData);
    std::string beginSigningLTA();
    void finishSigningLTA(const std::string &tsr, const std::optional<PoDoFo::ValidationData> &validationData);
    std::string getCrlFromCertificate(const std::string &base64Cert);
    std::string extractSignerCertFromTSR(const std::string &base64Tsr);
    std::string extractIssuerCertFromTSR(const std::string &base64Tsr);
    std::string getOCSPFromCertificate(const std::string &base64Cert, const std::string &base64IssuerCert);
    std::string buildOCSPRequestFromCertificates(const std::string &base64Cert, const std::string &base64IssuerCert);
    std::string getCertificateIssuerUrlFromCertificate(const std::string &base64Cert);
};

// Helper methods for JNI
std::string jstringToString(JNIEnv *env, jstring jStr);
std::vector<std::string> jstringArrayToVector(JNIEnv *env, jobjectArray jArray);
jstring stringToJstring(JNIEnv *env, const std::string &str);

// JNI function declarations
extern "C"
{
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeInit(
        JNIEnv *env, jobject thiz, jstring jConformanceLevel, jstring jHashAlgorithm,
        jstring jInputPath, jstring jOutputPath, jstring jCertificate,
        jobjectArray jChainCertificates);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeCleanup(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeIsLoaded(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativePrintState(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeCalculateHash(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeFinalizeSigningWithSignedHash(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jSignedHash, jstring jTsr,
        jobject jCertificates, jobject jCrls, jobject jOcsps);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeBeginSigningLTA(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeFinishSigningLTA(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jTsr,
        jobject jCertificates, jobject jCrls, jobject jOcsps);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetCrlFromCertificate(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Cert);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeExtractSignerCertFromTSR(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Tsr);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeExtractIssuerCertFromTSR(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Tsr);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetOCSPFromCertificate(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Cert, jstring jBase64IssuerCert);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeBuildOCSPRequestFromCertificates(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Cert, jstring jBase64IssuerCert);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetCertificateIssuerUrlFromCertificate(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jBase64Cert);

    // PdfDocument (PoDoFo::PdfMemDocument) — core document I/O + pages.
    // Unlike PoDoFoWrapper above, these operate directly on PoDoFo types via
    // reinterpret_cast, with no intermediate wrapper class: PdfMemDocument/
    // PdfPage already expose exactly what's needed, so there's no extra state
    // to track. nativeCreate/nativeLoad are `static native` in Java (no
    // receiver instance yet), hence `jclass` instead of `jobject` here.
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreate(
        JNIEnv *env, jclass clazz);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeLoad(
        JNIEnv *env, jclass clazz, jstring jPath, jstring jPassword);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSave(
        JNIEnv *env, jobject thiz, jlong handle, jstring jPath);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeClose(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetPageCount(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetPage(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreatePage(
        JNIEnv *env, jobject thiz, jlong handle, jdouble width, jdouble height);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeRemovePageAt(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetTitle(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetTitle(
        JNIEnv *env, jobject thiz, jlong handle, jstring jTitle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetAuthor(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetAuthor(
        JNIEnv *env, jobject thiz, jlong handle, jstring jAuthor);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetSubject(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetSubject(
        JNIEnv *env, jobject thiz, jlong handle, jstring jSubject);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetCreator(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetCreator(
        JNIEnv *env, jobject thiz, jlong handle, jstring jCreator);

    // PdfPage (PoDoFo::PdfPage) — non-owning: the page is owned by its parent
    // document's page tree, so there is deliberately no nativeClose/delete here.
    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetWidth(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetHeight(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetIndex(
        JNIEnv *env, jobject thiz, jlong handle);
}

#endif // PODOFO_JNI_H
