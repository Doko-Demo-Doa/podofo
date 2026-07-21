#ifndef PODOFO_JNI_H
#define PODOFO_JNI_H

#include <jni.h>
#include <podofo/podofo.h>
#include <optional>
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
                                                     const std::vector<std::string> &chainCertificates,
                                                     const std::optional<std::string> &rootCertificate);

    bool isLoaded() const;
    void printState() const;
    void setSignerName(const std::string &name);
    void setCreatingApplication(const std::string &application);
    void setSignatureLocation(const std::string &location);
    void setSignatureReason(const std::string &reason);
    void setSignatureContactInfo(const std::string &contactInfo);
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
        jobjectArray jChainCertificates, jstring jRootCertificate);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeCleanup(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeIsLoaded(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativePrintState(
        JNIEnv *env, jobject thiz, jlong nativeHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeSetSignerName(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jName);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeSetCreatingApplication(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jApplication);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeSetSignatureLocation(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jLocation);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeSetSignatureReason(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jReason);

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeSetSignatureContactInfo(
        JNIEnv *env, jobject thiz, jlong nativeHandle, jstring jContactInfo);

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

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfDocument_nativeIsEncrypted(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetEncryptionAlgorithm(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetEncryptionKeyLengthBits(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetEncryptionRevision(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfDocument_nativeIsMetadataEncrypted(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfDocument_nativeIsOwnerPasswordSet(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfDocument_nativeIsEncryptionParsed(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetEncryptionPermissions(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetPageCount(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetPage(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreatePage(
        JNIEnv *env, jobject thiz, jlong handle, jdouble width, jdouble height);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeRemovePageAt(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    // Creates a new page and inserts it at the given 0-based index in the
    // document's page tree (vs. nativeCreatePage which always appends).
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreatePageAt(
        JNIEnv *env, jobject thiz, jlong handle, jint index, jdouble width, jdouble height);

    // Merge: append every page from another PdfMemDocument into this one.
    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeAppendDocumentPages(
        JNIEnv *env, jobject thiz, jlong handle, jlong sourceHandle);

    // Merge/split: copy a contiguous range of pages from another PdfMemDocument
    // and append them at the end of this document. Split is the same call done
    // with an empty target document.
    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeAppendDocumentPagesRange(
        JNIEnv *env, jobject thiz, jlong handle, jlong sourceHandle, jint pageIndex, jint pageCount);

    // Insert a single page from another PdfMemDocument at the given index in
    // this document's page tree.
    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeInsertDocumentPageAt(
        JNIEnv *env, jobject thiz, jlong handle, jint atIndex, jlong sourceHandle, jint pageIndex);

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

    // Returns one of the 14 PDF standard fonts (see PdfStandard14FontType);
    // works without Fontconfig, unlike PoDoFo's by-name SearchFont(), since
    // PODOFO_WITH_FONTMANAGER=OFF for the Android build (see
    // scripts/android/podofo/build.sh). Non-owning, like PdfPage — the font
    // is owned by the document's font manager.
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetStandard14Font(
        JNIEnv *env, jobject thiz, jlong handle, jstring jName);

    // AcroForm fields — GetOrCreateAcroForm() is called internally so Java
    // callers don't need to think about the AcroForm indirection at all.
    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetFieldCount(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetFieldAt(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreateField(
        JNIEnv *env, jobject thiz, jlong handle, jstring jName, jstring jFieldType);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateOutlines(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateFont(
        JNIEnv *env, jobject thiz, jlong handle, jstring jFontFilePath);

    // Same as nativeGetOrCreateFont, but for a font already in memory (eg. copied
    // out of Android assets/) rather than one readable via a native file path.
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateFontFromBuffer(
        JNIEnv *env, jobject thiz, jlong handle, jbyteArray jData);

    // Returns an owning handle (see PdfImage.java) — PdfDocument::CreateImage()
    // returns a unique_ptr the caller must keep alive, unlike PdfPage/PdfField/
    // PdfAnnotation which are owned by an existing document-internal collection.
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreateImageFromBuffer(
        JNIEnv *env, jobject thiz, jlong handle, jbyteArray jData);

    // AES-256/R6 (PoDoFo's own default) only for now — see PdfDocument.java.
    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetEncrypted(
        JNIEnv *env, jobject thiz, jlong handle, jstring jUserPassword, jstring jOwnerPassword, jint permissions);

    // PdfPage (PoDoFo::PdfPage) — non-owning: the page is owned by its parent
    // document's page tree, so there is deliberately no nativeClose/delete here.
    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetWidth(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetHeight(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetIndex(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jobjectArray JNICALL Java_com_podofo_android_PdfPage_nativeExtractText(
        JNIEnv *env, jobject thiz, jlong handle, jstring jPattern);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetAnnotationCount(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPage_nativeGetAnnotationAt(
        JNIEnv *env, jobject thiz, jlong handle, jint index);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPage_nativeCreateAnnotation(
        JNIEnv *env, jobject thiz, jlong handle, jstring jAnnotationType,
        jdouble x, jdouble y, jdouble width, jdouble height);

    // Page rotation: GetRotation returns the normalized 0/90/180/270 clockwise
    // degrees stored in the page's /Rotate entry; SetRotation accepts any
    // multiple of 90 (negative values are normalized by PoDoFo).
    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetRotation(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetRotation(
        JNIEnv *env, jobject thiz, jlong handle, jint rotation);

    // Page resize via the /MediaBox. GetMediaBox returns {x, y, width, height}.
    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfPage_nativeGetMediaBox(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetMediaBox(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height);

    // /CropBox (visible page area). Same {x, y, width, height} layout as
    // the MediaBox accessors above.
    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfPage_nativeGetCropBox(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetCropBox(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height);

    // Reorder: moves this page to a new 0-based index within its parent
    // document's page tree. Wraps PdfPage::MoveTo, which delegates to
    // PdfPageCollection::TryMovePageTo.
    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfPage_nativeMoveTo(
        JNIEnv *env, jobject thiz, jlong handle, jint newIndex);

    // PdfPainter (PoDoFo::PdfPainter) — owning: unlike PdfPage/PdfFont, the
    // painter is not owned by the document, so nativeDestroy actually deletes.
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPainter_nativeCreate(
        JNIEnv *env, jclass clazz);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetCanvas(
        JNIEnv *env, jobject thiz, jlong handle, jlong pageHandle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDestroy(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeFinishDrawing(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetFont(
        JNIEnv *env, jobject thiz, jlong handle, jlong fontHandle, jdouble fontSize);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawText(
        JNIEnv *env, jobject thiz, jlong handle, jstring jText, jdouble x, jdouble y);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawImage(
        JNIEnv *env, jobject thiz, jlong handle, jlong imageHandle, jdouble x, jdouble y, jdouble scaleX, jdouble scaleY);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawLine(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x1, jdouble y1, jdouble x2, jdouble y2);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawRectangle(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height, jboolean fill);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawCircle(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble radius, jboolean fill);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetStrokingColorRGB(
        JNIEnv *env, jobject thiz, jlong handle, jdouble red, jdouble green, jdouble blue);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetNonStrokingColorRGB(
        JNIEnv *env, jobject thiz, jlong handle, jdouble red, jdouble green, jdouble blue);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSave(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeRestore(
        JNIEnv *env, jobject thiz, jlong handle);

    // PdfField (PoDoFo::PdfField) — non-owning, like PdfPage/PdfFont: owned
    // by the document's AcroForm. getText/isChecked/etc. dynamic_cast to the
    // concrete subclass (PdfTextBox/PdfToggleButton) and throw if the field
    // isn't actually that type, rather than exposing a Java class per
    // PdfField subclass.
    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetFieldType(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetFullName(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetText(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfField_nativeSetText(
        JNIEnv *env, jobject thiz, jlong handle, jstring jText);

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfField_nativeIsChecked(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfField_nativeSetChecked(
        JNIEnv *env, jobject thiz, jlong handle, jboolean checked);

    // PdfSignature (PoDoFo::PdfSignature) — non-owning, like PdfField.
    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfSignature_nativeHasSignatureValue(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetFilter(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetSubFilter(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetType(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetName(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetReason(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetLocation(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetContactInfo(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetSignDate(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlongArray JNICALL Java_com_podofo_android_PdfSignature_nativeGetByteRange(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetPropBuild(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jbyteArray JNICALL Java_com_podofo_android_PdfSignature_nativeGetContents(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jobject JNICALL Java_com_podofo_android_PdfSignature_nativeParseContents(
        JNIEnv *env, jobject thiz, jlong handle);

    // Returns a PdfSignatureVerifyStatus ordinal (see PdfSignature.VerifyStatus
    // in Java, which mirrors it exactly) as a jint.
    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfSignature_nativeVerifySignature(
        JNIEnv *env, jobject thiz, jlong handle, jbyteArray jSignedData);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfSignature_nativeVerifySignatureFromPath(
        JNIEnv *env, jobject thiz, jlong handle, jstring jDocumentPath);

    // PdfAnnotation (PoDoFo::PdfAnnotation) — non-owning, like PdfField:
    // owned by its page's annotation collection.
    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetAnnotationType(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetRect(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfAnnotation_nativeSetRect(
        JNIEnv *env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height);

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetContents(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfAnnotation_nativeSetContents(
        JNIEnv *env, jobject thiz, jlong handle, jstring jContents);

    // PdfOutlineItem (PoDoFo::PdfOutlineItem/PdfOutlines) — non-owning, like
    // PdfPage: owned by the document's outline tree.
    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetTitle(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfOutlineItem_nativeSetTitle(
        JNIEnv *env, jobject thiz, jlong handle, jstring jTitle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeCreateChild(
        JNIEnv *env, jobject thiz, jlong handle, jstring jTitle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeCreateNext(
        JNIEnv *env, jobject thiz, jlong handle, jstring jTitle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetFirst(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetNext(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetParent(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfOutlineItem_nativeSetDestinationToPage(
        JNIEnv *env, jobject thiz, jlong handle, jlong pageHandle);

    // PdfImage (PoDoFo::PdfImage) — owning, like PdfPainter: see PdfImage.java.
    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfImage_nativeGetWidth(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfImage_nativeGetHeight(
        JNIEnv *env, jobject thiz, jlong handle);

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfImage_nativeDestroy(
        JNIEnv *env, jobject thiz, jlong handle);
}

#endif // PODOFO_JNI_H
