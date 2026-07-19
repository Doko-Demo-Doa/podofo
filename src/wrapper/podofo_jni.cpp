#include "podofo_jni.h"
#include <android/log.h>

// PoDoFoWrapper implementation
std::unique_ptr<PoDoFoWrapper> PoDoFoWrapper::initialize(const std::string& conformanceLevel,
                                     const std::string& hashAlgorithm,
                                     const std::string& inputPath,
                                     const std::string& outputPath,
                                     const std::string& certificate,
                                     const std::vector<std::string>& chainCertificates) {
    try {
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "Creating PoDoFoWrapper instance");
        auto wrapper = std::make_unique<PoDoFoWrapper>();

        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "Creating PdfRemoteSignDocumentSession with parameters:");
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Conformance Level: %s", conformanceLevel.c_str());
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Hash Algorithm: %s", hashAlgorithm.c_str());
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Input Path: %s", inputPath.c_str());
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Output Path: %s", outputPath.c_str());
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Certificate length: %zu", certificate.length());
        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "  Chain Certificates count: %zu", chainCertificates.size());

        wrapper->nativeSession = std::make_unique<PoDoFo::PdfRemoteSignDocumentSession>(
            conformanceLevel,
            hashAlgorithm,
            inputPath,
            outputPath,
            certificate,
            chainCertificates,
            std::nullopt
        );

        __android_log_print(ANDROID_LOG_INFO, "PoDoFo", "PdfRemoteSignDocumentSession created successfully");
        return wrapper;
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception during initialization: %s", e.what());
        return nullptr;
    } catch (...) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Unknown exception during initialization");
        return nullptr;
    }
}

bool PoDoFoWrapper::isLoaded() const {
    return nativeSession != nullptr;
}

void PoDoFoWrapper::printState() const {
    if (nativeSession) {
        try {
            nativeSession->printState();
        } catch (const std::exception& e) {
            // Handle exception silently
        }
    }
}

std::string PoDoFoWrapper::calculateHash() {
    if (!nativeSession) {
        throw std::runtime_error("PoDoFo session is not initialized.");
    }

    try {
        return nativeSession->beginSigning();
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in calculateHash: %s", e.what());
        throw;
    }
}

void PoDoFoWrapper::finalizeSigningWithSignedHash(const std::string& signedHash, const std::string& tsr, const std::optional<PoDoFo::ValidationData>& validationData) {
    if (!nativeSession) {
        throw std::runtime_error("PoDoFo session is not initialized.");
    }

    if (signedHash.empty()) {
        return;
    }

    try {
        nativeSession->finishSigning(signedHash, tsr, validationData);
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in finalizeSigningWithSignedHash: %s", e.what());
        throw;
    }
}

std::string PoDoFoWrapper::beginSigningLTA() {
    if (!nativeSession) {
        throw std::runtime_error("PoDoFo session is not initialized.");
    }

    try {
        return nativeSession->beginSigningLTA();
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in beginSigningLTA: %s", e.what());
        throw;
    }
}

void PoDoFoWrapper::finishSigningLTA(const std::string& tsr, const std::optional<PoDoFo::ValidationData>& validationData) {
    if (!nativeSession) {
        throw std::runtime_error("PoDoFo session is not initialized.");
    }

    try {
        nativeSession->finishSigningLTA(tsr, validationData);
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in finishSigningLTA: %s", e.what());
        throw;
    }
}

std::string PoDoFoWrapper::getCrlFromCertificate(const std::string& base64Cert) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->getCrlFromCertificate(base64Cert);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in getCrlFromCertificate: %s", e.what());
		throw;
	}
}

std::string PoDoFoWrapper::extractSignerCertFromTSR(const std::string& base64Tsr) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->extractSignerCertFromTSR(base64Tsr);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in extractSignerCertFromTSR: %s", e.what());
		throw;
	}
}

std::string PoDoFoWrapper::extractIssuerCertFromTSR(const std::string& base64Tsr) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->extractIssuerCertFromTSR(base64Tsr);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in extractIssuerCertFromTSR: %s", e.what());
		throw;
	}
}

std::string PoDoFoWrapper::getOCSPFromCertificate(const std::string& base64Cert, const std::string& base64IssuerCert) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->getOCSPFromCertificate(base64Cert, base64IssuerCert);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in getOCSPFromCertificate: %s", e.what());
		throw;
	}
}

std::string PoDoFoWrapper::buildOCSPRequestFromCertificates(const std::string& base64Cert, const std::string& base64IssuerCert) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->buildOCSPRequestFromCertificates(base64Cert, base64IssuerCert);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in buildOCSPRequestFromCertificates: %s", e.what());
		throw;
	}
}

std::string PoDoFoWrapper::getCertificateIssuerUrlFromCertificate(const std::string& base64Cert) {
	if (!nativeSession) {
		throw std::runtime_error("PoDoFo session is not initialized.");
	}

	try {
		return nativeSession->getCertificateIssuerUrlFromCertificate(base64Cert);
	} catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception in getCertificateIssuerUrlFromCertificate: %s", e.what());
		throw;
	}
}

// Helper functions
void throwJavaException(JNIEnv* env, const char* message) {
    jclass exceptionClass = env->FindClass("com/podofo/android/PoDoFoException");
    if (exceptionClass != NULL) {
        env->ThrowNew(exceptionClass, message);
    }
}

std::string jstringToString(JNIEnv* env, jstring jStr) {
    if (!jStr) {
        return "";
    }

    const char* cStr = env->GetStringUTFChars(jStr, nullptr);
    if (!cStr) {
        return "";
    }

    std::string str(cStr);
    env->ReleaseStringUTFChars(jStr, cStr);
    return str;
}

std::vector<std::string> jstringArrayToVector(JNIEnv* env, jobjectArray jArray) {
    std::vector<std::string> result;

    if (!jArray) {
        return result;
    }

    jsize length = env->GetArrayLength(jArray);
    result.reserve(length);

    for (jsize i = 0; i < length; i++) {
        jstring jStr = (jstring)env->GetObjectArrayElement(jArray, i);
        if (jStr) {
            result.push_back(jstringToString(env, jStr));
            env->DeleteLocalRef(jStr);
        }
    }

    return result;
}

std::vector<std::string> jlistToVector(JNIEnv* env, jobject jList) {
    std::vector<std::string> result;
    if (!jList) {
        return result;
    }

    jclass listClass = env->GetObjectClass(jList);
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

    jint size = env->CallIntMethod(jList, sizeMethod);
    result.reserve(size);

    for (jint i = 0; i < size; i++) {
        jstring jStr = (jstring)env->CallObjectMethod(jList, getMethod, i);
        if (jStr) {
            result.push_back(jstringToString(env, jStr));
            env->DeleteLocalRef(jStr);
        }
    }

    return result;
}

jstring stringToJstring(JNIEnv* env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

// JNI functions implementation
extern "C" {
    JNIEXPORT jlong JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeInit(
        JNIEnv* env, jobject thiz, jstring jConformanceLevel, jstring jHashAlgorithm,
        jstring jInputPath, jstring jOutputPath, jstring jCertificate,
        jobjectArray jChainCertificates) {

        try {
            // Convert Java strings to C++ strings
            std::string conformanceLevel = jstringToString(env, jConformanceLevel);
            std::string hashAlgorithm = jstringToString(env, jHashAlgorithm);
            std::string inputPath = jstringToString(env, jInputPath);
            std::string outputPath = jstringToString(env, jOutputPath);
            std::string certificate = jstringToString(env, jCertificate);
            std::vector<std::string> chainCertificates = jstringArrayToVector(env, jChainCertificates);

            // Create the wrapper using the static factory method
            auto wrapper_unique = PoDoFoWrapper::initialize(
                conformanceLevel, hashAlgorithm, inputPath, outputPath,
                certificate, chainCertificates);

            if (!wrapper_unique) {
                throwJavaException(env, "Failed to initialize native PoDoFo wrapper");
                return 0;
            }

            // Transfer ownership from unique_ptr to raw pointer for JNI
            PoDoFoWrapper* wrapper = wrapper_unique.release();
            return reinterpret_cast<jlong>(wrapper);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        } catch (...) {
            throwJavaException(env, "Unknown exception during initialization");
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeCleanup(
        JNIEnv* env, jobject thiz, jlong nativeHandle) {

        if (nativeHandle) {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            delete wrapper;
        }
    }

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeIsLoaded(
        JNIEnv* env, jobject thiz, jlong nativeHandle) {

        if (!nativeHandle) {
            return JNI_FALSE;
        }

        auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
        return wrapper->isLoaded() ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativePrintState(
        JNIEnv* env, jobject thiz, jlong nativeHandle) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return;
        }

        auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
        wrapper->printState();
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeCalculateHash(
        JNIEnv* env, jobject thiz, jlong nativeHandle) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string hash = wrapper->calculateHash();
            return hash.empty() ? nullptr : stringToJstring(env, hash);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeFinalizeSigningWithSignedHash(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jSignedHash, jstring jTsr,
        jobject jCertificates, jobject jCrls, jobject jOcsps) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return;
        }

        if (!jSignedHash) {
            throwJavaException(env, "Signed hash is null");
            return;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string signedHash = jstringToString(env, jSignedHash);
            std::string tsr = jstringToString(env, jTsr);

            std::optional<PoDoFo::ValidationData> validationData;
            if (jCertificates || jCrls || jOcsps) {
                PoDoFo::ValidationData cppValidationData;
                if (jCertificates) {
                    for (const auto& cert : jlistToVector(env, jCertificates)) {
                        cppValidationData.addCertificate(cert);
                    }
                }
                if (jCrls) {
                    for (const auto& crl : jlistToVector(env, jCrls)) {
                        cppValidationData.addCRL(crl);
                    }
                }
                if (jOcsps) {
                    for (const auto& ocsp : jlistToVector(env, jOcsps)) {
                        cppValidationData.addOCSP(ocsp);
                    }
                }
                validationData = cppValidationData;
            }

            wrapper->finalizeSigningWithSignedHash(signedHash, tsr, validationData);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeBeginSigningLTA(
        JNIEnv* env, jobject thiz, jlong nativeHandle) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string hash = wrapper->beginSigningLTA();
            return hash.empty() ? nullptr : stringToJstring(env, hash);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeFinishSigningLTA(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jTsr,
        jobject jCertificates, jobject jCrls, jobject jOcsps) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string tsr = jstringToString(env, jTsr);

            std::optional<PoDoFo::ValidationData> validationData;
            if (jCertificates || jCrls || jOcsps) {
                PoDoFo::ValidationData cppValidationData;
                if (jCertificates) {
                    for (const auto& cert : jlistToVector(env, jCertificates)) {
                        cppValidationData.addCertificate(cert);
                    }
                }
                if (jCrls) {
                    for (const auto& crl : jlistToVector(env, jCrls)) {
                        cppValidationData.addCRL(crl);
                    }
                }
                if (jOcsps) {
                    for (const auto& ocsp : jlistToVector(env, jOcsps)) {
                        cppValidationData.addOCSP(ocsp);
                    }
                }
                validationData = cppValidationData;
            }

            wrapper->finishSigningLTA(tsr, validationData);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetCrlFromCertificate(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Cert) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Cert) {
            throwJavaException(env, "Certificate is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Cert = jstringToString(env, jBase64Cert);
            std::string crlUrl = wrapper->getCrlFromCertificate(base64Cert);
            return crlUrl.empty() ? nullptr : stringToJstring(env, crlUrl);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeExtractSignerCertFromTSR(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Tsr) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Tsr) {
            throwJavaException(env, "TSR is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Tsr = jstringToString(env, jBase64Tsr);
            std::string signerCert = wrapper->extractSignerCertFromTSR(base64Tsr);
            return signerCert.empty() ? nullptr : stringToJstring(env, signerCert);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeExtractIssuerCertFromTSR(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Tsr) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Tsr) {
            throwJavaException(env, "TSR is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Tsr = jstringToString(env, jBase64Tsr);
            std::string issuerCert = wrapper->extractIssuerCertFromTSR(base64Tsr);
            return issuerCert.empty() ? nullptr : stringToJstring(env, issuerCert);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetOCSPFromCertificate(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Cert, jstring jBase64IssuerCert) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Cert) {
            throwJavaException(env, "Certificate is null");
            return nullptr;
        }

        if (!jBase64IssuerCert) {
            throwJavaException(env, "Issuer certificate is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Cert = jstringToString(env, jBase64Cert);
            std::string base64IssuerCert = jstringToString(env, jBase64IssuerCert);
            std::string ocspUrl = wrapper->getOCSPFromCertificate(base64Cert, base64IssuerCert);
            return ocspUrl.empty() ? nullptr : stringToJstring(env, ocspUrl);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeBuildOCSPRequestFromCertificates(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Cert, jstring jBase64IssuerCert) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Cert) {
            throwJavaException(env, "Certificate is null");
            return nullptr;
        }

        if (!jBase64IssuerCert) {
            throwJavaException(env, "Issuer certificate is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Cert = jstringToString(env, jBase64Cert);
            std::string base64IssuerCert = jstringToString(env, jBase64IssuerCert);
            std::string ocspRequest = wrapper->buildOCSPRequestFromCertificates(base64Cert, base64IssuerCert);
            return ocspRequest.empty() ? nullptr : stringToJstring(env, ocspRequest);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PoDoFoWrapper_nativeGetCertificateIssuerUrlFromCertificate(
        JNIEnv* env, jobject thiz, jlong nativeHandle, jstring jBase64Cert) {

        if (!nativeHandle) {
            throwJavaException(env, "Session not initialized");
            return nullptr;
        }

        if (!jBase64Cert) {
            throwJavaException(env, "Certificate is null");
            return nullptr;
        }

        try {
            auto* wrapper = reinterpret_cast<PoDoFoWrapper*>(nativeHandle);
            std::string base64Cert = jstringToString(env, jBase64Cert);
            std::string issuerUrl = wrapper->getCertificateIssuerUrlFromCertificate(base64Cert);
            return issuerUrl.empty() ? nullptr : stringToJstring(env, issuerUrl);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    // ---- PdfDocument / PdfPage ----

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreate(
        JNIEnv* env, jclass clazz) {

        try {
            auto* doc = new PoDoFo::PdfMemDocument();
            return reinterpret_cast<jlong>(doc);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeLoad(
        JNIEnv* env, jclass clazz, jstring jPath, jstring jPassword) {

        auto* doc = new PoDoFo::PdfMemDocument();
        try {
            std::string path = jstringToString(env, jPath);
            std::string password = jPassword ? jstringToString(env, jPassword) : std::string();
            doc->Load(path, password);
            return reinterpret_cast<jlong>(doc);
        } catch (const std::exception& e) {
            delete doc;
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSave(
        JNIEnv* env, jobject thiz, jlong handle, jstring jPath) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            doc->Save(jstringToString(env, jPath));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeClose(
        JNIEnv* env, jobject thiz, jlong handle) {

        if (handle) {
            delete reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetPageCount(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        return static_cast<jint>(doc->GetPages().GetCount());
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetPage(
        JNIEnv* env, jobject thiz, jlong handle, jint index) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto& page = doc->GetPages().GetPageAt(static_cast<unsigned>(index));
            return reinterpret_cast<jlong>(&page);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreatePage(
        JNIEnv* env, jobject thiz, jlong handle, jdouble width, jdouble height) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto& page = doc->GetPages().CreatePage(PoDoFo::Rect(0, 0, width, height));
            return reinterpret_cast<jlong>(&page);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeRemovePageAt(
        JNIEnv* env, jobject thiz, jlong handle, jint index) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            doc->GetPages().RemovePageAt(static_cast<unsigned>(index));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreatePageAt(
        JNIEnv* env, jobject thiz, jlong handle, jint index, jdouble width, jdouble height) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto& page = doc->GetPages().CreatePageAt(static_cast<unsigned>(index),
                PoDoFo::Rect(0, 0, width, height));
            return reinterpret_cast<jlong>(&page);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeAppendDocumentPages(
        JNIEnv* env, jobject thiz, jlong handle, jlong sourceHandle) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto* source = reinterpret_cast<PoDoFo::PdfMemDocument*>(sourceHandle);
            doc->GetPages().AppendDocumentPages(*source);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeAppendDocumentPagesRange(
        JNIEnv* env, jobject thiz, jlong handle, jlong sourceHandle, jint pageIndex, jint pageCount) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto* source = reinterpret_cast<PoDoFo::PdfMemDocument*>(sourceHandle);
            doc->GetPages().AppendDocumentPages(*source,
                static_cast<unsigned>(pageIndex), static_cast<unsigned>(pageCount));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeInsertDocumentPageAt(
        JNIEnv* env, jobject thiz, jlong handle, jint atIndex, jlong sourceHandle, jint pageIndex) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto* source = reinterpret_cast<PoDoFo::PdfMemDocument*>(sourceHandle);
            doc->GetPages().InsertDocumentPageAt(static_cast<unsigned>(atIndex),
                *source, static_cast<unsigned>(pageIndex));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetTitle(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        auto title = doc->GetMetadata().GetTitle();
        return title.has_value() ? stringToJstring(env, std::string(title.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetTitle(
        JNIEnv* env, jobject thiz, jlong handle, jstring jTitle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        if (jTitle == nullptr) {
            doc->GetMetadata().SetTitle(nullptr);
        } else {
            doc->GetMetadata().SetTitle(PoDoFo::PdfString(jstringToString(env, jTitle)));
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetAuthor(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        auto author = doc->GetMetadata().GetAuthor();
        return author.has_value() ? stringToJstring(env, std::string(author.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetAuthor(
        JNIEnv* env, jobject thiz, jlong handle, jstring jAuthor) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        if (jAuthor == nullptr) {
            doc->GetMetadata().SetAuthor(nullptr);
        } else {
            doc->GetMetadata().SetAuthor(PoDoFo::PdfString(jstringToString(env, jAuthor)));
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetSubject(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        auto subject = doc->GetMetadata().GetSubject();
        return subject.has_value() ? stringToJstring(env, std::string(subject.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetSubject(
        JNIEnv* env, jobject thiz, jlong handle, jstring jSubject) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        if (jSubject == nullptr) {
            doc->GetMetadata().SetSubject(nullptr);
        } else {
            doc->GetMetadata().SetSubject(PoDoFo::PdfString(jstringToString(env, jSubject)));
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfDocument_nativeGetCreator(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        auto creator = doc->GetMetadata().GetCreator();
        return creator.has_value() ? stringToJstring(env, std::string(creator.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetCreator(
        JNIEnv* env, jobject thiz, jlong handle, jstring jCreator) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        if (jCreator == nullptr) {
            doc->GetMetadata().SetCreator(nullptr);
        } else {
            doc->GetMetadata().SetCreator(PoDoFo::PdfString(jstringToString(env, jCreator)));
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetStandard14Font(
        JNIEnv* env, jobject thiz, jlong handle, jstring jName) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            std::string name = jstringToString(env, jName);

            PoDoFo::PdfStandard14FontType fontType;
            if (name == "TimesRoman") fontType = PoDoFo::PdfStandard14FontType::TimesRoman;
            else if (name == "TimesItalic") fontType = PoDoFo::PdfStandard14FontType::TimesItalic;
            else if (name == "TimesBold") fontType = PoDoFo::PdfStandard14FontType::TimesBold;
            else if (name == "TimesBoldItalic") fontType = PoDoFo::PdfStandard14FontType::TimesBoldItalic;
            else if (name == "Helvetica") fontType = PoDoFo::PdfStandard14FontType::Helvetica;
            else if (name == "HelveticaOblique") fontType = PoDoFo::PdfStandard14FontType::HelveticaOblique;
            else if (name == "HelveticaBold") fontType = PoDoFo::PdfStandard14FontType::HelveticaBold;
            else if (name == "HelveticaBoldOblique") fontType = PoDoFo::PdfStandard14FontType::HelveticaBoldOblique;
            else if (name == "Courier") fontType = PoDoFo::PdfStandard14FontType::Courier;
            else if (name == "CourierOblique") fontType = PoDoFo::PdfStandard14FontType::CourierOblique;
            else if (name == "CourierBold") fontType = PoDoFo::PdfStandard14FontType::CourierBold;
            else if (name == "CourierBoldOblique") fontType = PoDoFo::PdfStandard14FontType::CourierBoldOblique;
            else if (name == "Symbol") fontType = PoDoFo::PdfStandard14FontType::Symbol;
            else if (name == "ZapfDingbats") fontType = PoDoFo::PdfStandard14FontType::ZapfDingbats;
            else {
                throwJavaException(env, ("Unknown standard font: " + name).c_str());
                return 0;
            }

            auto& font = doc->GetFonts().GetStandard14Font(fontType);
            return reinterpret_cast<jlong>(&font);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfDocument_nativeGetFieldCount(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto* form = doc->GetAcroForm();
            return form == nullptr ? 0 : static_cast<jint>(form->GetFieldCount());
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetFieldAt(
        JNIEnv* env, jobject thiz, jlong handle, jint index) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto& form = doc->GetOrCreateAcroForm();
            auto& field = form.GetFieldAt(static_cast<unsigned>(index));
            return reinterpret_cast<jlong>(&field);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreateField(
        JNIEnv* env, jobject thiz, jlong handle, jstring jName, jstring jFieldType) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            std::string name = jstringToString(env, jName);
            std::string fieldTypeName = jstringToString(env, jFieldType);

            PoDoFo::PdfFieldType fieldType;
            if (fieldTypeName == "TextBox") fieldType = PoDoFo::PdfFieldType::TextBox;
            else if (fieldTypeName == "CheckBox") fieldType = PoDoFo::PdfFieldType::CheckBox;
            else if (fieldTypeName == "RadioButton") fieldType = PoDoFo::PdfFieldType::RadioButton;
            else if (fieldTypeName == "PushButton") fieldType = PoDoFo::PdfFieldType::PushButton;
            else if (fieldTypeName == "ComboBox") fieldType = PoDoFo::PdfFieldType::ComboBox;
            else if (fieldTypeName == "ListBox") fieldType = PoDoFo::PdfFieldType::ListBox;
            else {
                throwJavaException(env, ("Unknown field type: " + fieldTypeName).c_str());
                return 0;
            }

            auto& form = doc->GetOrCreateAcroForm();
            auto& field = form.CreateField(name, fieldType);
            return reinterpret_cast<jlong>(&field);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateOutlines(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            auto& outlines = doc->GetOrCreateOutlines();
            return reinterpret_cast<jlong>(&outlines);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateFont(
        JNIEnv* env, jobject thiz, jlong handle, jstring jFontFilePath) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            std::string fontPath = jstringToString(env, jFontFilePath);
            auto& font = doc->GetFonts().GetOrCreateFont(fontPath);
            return reinterpret_cast<jlong>(&font);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeGetOrCreateFontFromBuffer(
        JNIEnv* env, jobject thiz, jlong handle, jbyteArray jData) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            jsize length = env->GetArrayLength(jData);
            jbyte* bytes = env->GetByteArrayElements(jData, nullptr);
            if (bytes == nullptr) {
                throwJavaException(env, "Failed to access font data");
                return 0;
            }

            PoDoFo::PdfFont* font = nullptr;
            try {
                PoDoFo::bufferview buffer(reinterpret_cast<const char*>(bytes), static_cast<size_t>(length));
                font = &doc->GetFonts().GetOrCreateFontFromBuffer(buffer);
            } catch (...) {
                env->ReleaseByteArrayElements(jData, bytes, JNI_ABORT);
                throw;
            }
            env->ReleaseByteArrayElements(jData, bytes, JNI_ABORT);

            return reinterpret_cast<jlong>(font);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfDocument_nativeCreateImageFromBuffer(
        JNIEnv* env, jobject thiz, jlong handle, jbyteArray jData) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            jsize length = env->GetArrayLength(jData);
            jbyte* bytes = env->GetByteArrayElements(jData, nullptr);
            if (bytes == nullptr) {
                throwJavaException(env, "Failed to access image data");
                return 0;
            }

            std::unique_ptr<PoDoFo::PdfImage> image;
            try {
                image = doc->CreateImage();
                PoDoFo::bufferview buffer(reinterpret_cast<const char*>(bytes), static_cast<size_t>(length));
                image->LoadFromBuffer(buffer);
            } catch (...) {
                env->ReleaseByteArrayElements(jData, bytes, JNI_ABORT);
                throw;
            }
            env->ReleaseByteArrayElements(jData, bytes, JNI_ABORT);

            return reinterpret_cast<jlong>(image.release());
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfDocument_nativeSetEncrypted(
        JNIEnv* env, jobject thiz, jlong handle, jstring jUserPassword, jstring jOwnerPassword, jint permissions) {

        try {
            auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
            std::string userPassword = jstringToString(env, jUserPassword);
            std::string ownerPassword = jstringToString(env, jOwnerPassword);
            doc->SetEncrypted(userPassword, ownerPassword,
                static_cast<PoDoFo::PdfPermissions>(permissions));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfDocument_nativeIsEncrypted(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* doc = reinterpret_cast<PoDoFo::PdfMemDocument*>(handle);
        return doc->IsEncrypted() ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetWidth(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        return page->GetRect().Width;
    }

    JNIEXPORT jdouble JNICALL Java_com_podofo_android_PdfPage_nativeGetHeight(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        return page->GetRect().Height;
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetIndex(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        return static_cast<jint>(page->GetIndex());
    }

    JNIEXPORT jobjectArray JNICALL Java_com_podofo_android_PdfPage_nativeExtractText(
        JNIEnv* env, jobject thiz, jlong handle, jstring jPattern) {

        try {
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
            std::vector<PoDoFo::PdfTextEntry> entries;
            if (jPattern == nullptr) {
                page->ExtractTextTo(entries);
            } else {
                page->ExtractTextTo(entries, jstringToString(env, jPattern));
            }

            jclass entryClass = env->FindClass("com/podofo/android/PdfTextEntry");
            if (entryClass == nullptr) {
                throwJavaException(env, "Could not find PdfTextEntry class");
                return nullptr;
            }
            jmethodID ctor = env->GetMethodID(entryClass, "<init>", "(Ljava/lang/String;IDDD)V");
            if (ctor == nullptr) {
                throwJavaException(env, "Could not find PdfTextEntry constructor");
                return nullptr;
            }

            jobjectArray result = env->NewObjectArray(static_cast<jsize>(entries.size()), entryClass, nullptr);
            for (size_t i = 0; i < entries.size(); i++) {
                const auto& entry = entries[i];
                jstring jText = stringToJstring(env, entry.Text);
                jobject jEntry = env->NewObject(entryClass, ctor, jText,
                    static_cast<jint>(entry.Page), static_cast<jdouble>(entry.X),
                    static_cast<jdouble>(entry.Y), static_cast<jdouble>(entry.Length));
                env->SetObjectArrayElement(result, static_cast<jsize>(i), jEntry);
                env->DeleteLocalRef(jEntry);
                env->DeleteLocalRef(jText);
            }
            return result;
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetAnnotationCount(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        return static_cast<jint>(page->GetAnnotations().GetCount());
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPage_nativeGetAnnotationAt(
        JNIEnv* env, jobject thiz, jlong handle, jint index) {

        try {
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
            auto& annot = page->GetAnnotations().GetAnnotAt(static_cast<unsigned>(index));
            return reinterpret_cast<jlong>(&annot);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPage_nativeCreateAnnotation(
        JNIEnv* env, jobject thiz, jlong handle, jstring jAnnotationType,
        jdouble x, jdouble y, jdouble width, jdouble height) {

        try {
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
            std::string typeName = jstringToString(env, jAnnotationType);

            PoDoFo::PdfAnnotationType annotType;
            if (typeName == "Text") annotType = PoDoFo::PdfAnnotationType::Text;
            else if (typeName == "Link") annotType = PoDoFo::PdfAnnotationType::Link;
            else if (typeName == "FreeText") annotType = PoDoFo::PdfAnnotationType::FreeText;
            else if (typeName == "Line") annotType = PoDoFo::PdfAnnotationType::Line;
            else if (typeName == "Square") annotType = PoDoFo::PdfAnnotationType::Square;
            else if (typeName == "Circle") annotType = PoDoFo::PdfAnnotationType::Circle;
            else if (typeName == "Highlight") annotType = PoDoFo::PdfAnnotationType::Highlight;
            else if (typeName == "Underline") annotType = PoDoFo::PdfAnnotationType::Underline;
            else if (typeName == "Squiggly") annotType = PoDoFo::PdfAnnotationType::Squiggly;
            else if (typeName == "StrikeOut") annotType = PoDoFo::PdfAnnotationType::StrikeOut;
            else if (typeName == "Stamp") annotType = PoDoFo::PdfAnnotationType::Stamp;
            else if (typeName == "Ink") annotType = PoDoFo::PdfAnnotationType::Ink;
            else if (typeName == "Popup") annotType = PoDoFo::PdfAnnotationType::Popup;
            else {
                throwJavaException(env, ("Unknown or unsupported annotation type: " + typeName).c_str());
                return 0;
            }

            auto& annot = page->GetAnnotations().CreateAnnot(annotType, PoDoFo::Rect(x, y, width, height));
            return reinterpret_cast<jlong>(&annot);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfPage_nativeGetRotation(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        return static_cast<jint>(page->GetRotation());
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetRotation(
        JNIEnv* env, jobject thiz, jlong handle, jint rotation) {

        try {
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
            page->SetRotation(static_cast<int>(rotation));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfPage_nativeGetMediaBox(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        auto rect = page->GetMediaBox();
        jdoubleArray result = env->NewDoubleArray(4);
        jdouble values[4] = { rect.X, rect.Y, rect.Width, rect.Height };
        env->SetDoubleArrayRegion(result, 0, 4, values);
        return result;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetMediaBox(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        page->SetMediaBox(PoDoFo::Rect(x, y, width, height));
    }

    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfPage_nativeGetCropBox(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        auto rect = page->GetCropBox();
        jdoubleArray result = env->NewDoubleArray(4);
        jdouble values[4] = { rect.X, rect.Y, rect.Width, rect.Height };
        env->SetDoubleArrayRegion(result, 0, 4, values);
        return result;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPage_nativeSetCropBox(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height) {

        auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
        page->SetCropBox(PoDoFo::Rect(x, y, width, height));
    }

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfPage_nativeMoveTo(
        JNIEnv* env, jobject thiz, jlong handle, jint newIndex) {

        try {
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(handle);
            bool moved = page->MoveTo(static_cast<unsigned>(newIndex));
            return moved ? JNI_TRUE : JNI_FALSE;
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return JNI_FALSE;
        }
    }

    // ---- PdfPainter ----

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfPainter_nativeCreate(
        JNIEnv* env, jclass clazz) {

        try {
            auto* painter = new PoDoFo::PdfPainter();
            return reinterpret_cast<jlong>(painter);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetCanvas(
        JNIEnv* env, jobject thiz, jlong handle, jlong pageHandle) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(pageHandle);
            painter->SetCanvas(*page);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDestroy(
        JNIEnv* env, jobject thiz, jlong handle) {

        // ~PdfPainter() is `noexcept(false)`: it can throw if FinishDrawing()
        // wasn't called first. Swallow rather than let a C++ exception escape
        // across the JNI boundary during what's meant to be a cleanup call.
        try {
            delete reinterpret_cast<PoDoFo::PdfPainter*>(handle);
        } catch (const std::exception& e) {
            __android_log_print(ANDROID_LOG_ERROR, "PoDoFo", "Exception destroying PdfPainter: %s", e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeFinishDrawing(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->FinishDrawing();
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetFont(
        JNIEnv* env, jobject thiz, jlong handle, jlong fontHandle, jdouble fontSize) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            auto* font = reinterpret_cast<PoDoFo::PdfFont*>(fontHandle);
            painter->TextState.SetFont(*font, fontSize);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawText(
        JNIEnv* env, jobject thiz, jlong handle, jstring jText, jdouble x, jdouble y) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->DrawText(jstringToString(env, jText), x, y);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawImage(
        JNIEnv* env, jobject thiz, jlong handle, jlong imageHandle, jdouble x, jdouble y, jdouble scaleX, jdouble scaleY) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            auto* image = reinterpret_cast<PoDoFo::PdfImage*>(imageHandle);
            painter->DrawImage(*image, x, y, scaleX, scaleY);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawLine(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x1, jdouble y1, jdouble x2, jdouble y2) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->DrawLine(x1, y1, x2, y2);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawRectangle(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height, jboolean fill) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            auto mode = fill ? PoDoFo::PdfPathDrawMode::Fill : PoDoFo::PdfPathDrawMode::Stroke;
            painter->DrawRectangle(x, y, width, height, mode);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeDrawCircle(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble radius, jboolean fill) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            auto mode = fill ? PoDoFo::PdfPathDrawMode::Fill : PoDoFo::PdfPathDrawMode::Stroke;
            painter->DrawCircle(x, y, radius, mode);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetStrokingColorRGB(
        JNIEnv* env, jobject thiz, jlong handle, jdouble red, jdouble green, jdouble blue) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->GraphicsState.SetStrokingColor(PoDoFo::PdfColor(red, green, blue));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSetNonStrokingColorRGB(
        JNIEnv* env, jobject thiz, jlong handle, jdouble red, jdouble green, jdouble blue) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->GraphicsState.SetNonStrokingColor(PoDoFo::PdfColor(red, green, blue));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeSave(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->Save();
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfPainter_nativeRestore(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* painter = reinterpret_cast<PoDoFo::PdfPainter*>(handle);
            painter->Restore();
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    // ---- PdfField ----

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetFieldType(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
        switch (field->GetType()) {
            case PoDoFo::PdfFieldType::PushButton: return stringToJstring(env, "PushButton");
            case PoDoFo::PdfFieldType::CheckBox: return stringToJstring(env, "CheckBox");
            case PoDoFo::PdfFieldType::RadioButton: return stringToJstring(env, "RadioButton");
            case PoDoFo::PdfFieldType::TextBox: return stringToJstring(env, "TextBox");
            case PoDoFo::PdfFieldType::ComboBox: return stringToJstring(env, "ComboBox");
            case PoDoFo::PdfFieldType::ListBox: return stringToJstring(env, "ListBox");
            case PoDoFo::PdfFieldType::Signature: return stringToJstring(env, "Signature");
            default: return stringToJstring(env, "Unknown");
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetFullName(
        JNIEnv* env, jobject thiz, jlong handle) {

        try {
            auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
            return stringToJstring(env, field->GetFullName());
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfField_nativeGetText(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
        auto* textBox = dynamic_cast<PoDoFo::PdfTextBox*>(field);
        if (textBox == nullptr) {
            throwJavaException(env, "Field is not a TextBox");
            return nullptr;
        }
        auto text = textBox->GetText();
        return text.has_value() ? stringToJstring(env, std::string(text.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfField_nativeSetText(
        JNIEnv* env, jobject thiz, jlong handle, jstring jText) {

        auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
        auto* textBox = dynamic_cast<PoDoFo::PdfTextBox*>(field);
        if (textBox == nullptr) {
            throwJavaException(env, "Field is not a TextBox");
            return;
        }
        if (jText == nullptr) {
            textBox->SetText(nullptr);
        } else {
            textBox->SetText(PoDoFo::PdfString(jstringToString(env, jText)));
        }
    }

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfField_nativeIsChecked(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
        auto* toggle = dynamic_cast<PoDoFo::PdfToggleButton*>(field);
        if (toggle == nullptr) {
            throwJavaException(env, "Field is not a CheckBox/RadioButton");
            return JNI_FALSE;
        }
        return toggle->IsChecked() ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfField_nativeSetChecked(
        JNIEnv* env, jobject thiz, jlong handle, jboolean checked) {

        auto* field = reinterpret_cast<PoDoFo::PdfField*>(handle);
        auto* toggle = dynamic_cast<PoDoFo::PdfToggleButton*>(field);
        if (toggle == nullptr) {
            throwJavaException(env, "Field is not a CheckBox/RadioButton");
            return;
        }
        toggle->SetChecked(checked == JNI_TRUE);
    }

    // ---- PdfSignature ----

    static jobject createPdfSignatureContents(JNIEnv* env, const PoDoFo::PdfSignatureContents& contents)
    {
        jclass contentsClass = env->FindClass("com/podofo/android/PdfSignatureContents");
        if (contentsClass == nullptr)
            return nullptr;

        jclass signerInfoClass = env->FindClass("com/podofo/android/PdfSignatureContents$SignerInfo");
        if (signerInfoClass == nullptr)
            return nullptr;

        jclass timestampInfoClass = env->FindClass("com/podofo/android/PdfSignatureContents$TimestampInfo");
        if (timestampInfoClass == nullptr)
            return nullptr;

        jclass arrayListClass = env->FindClass("java/util/ArrayList");
        if (arrayListClass == nullptr)
            return nullptr;

        jmethodID contentsCtor = env->GetMethodID(contentsClass, "<init>",
            "(Ljava/util/List;Lcom/podofo/android/PdfSignatureContents$TimestampInfo;)V");
        jmethodID signerInfoCtor = env->GetMethodID(signerInfoClass, "<init>",
            "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        jmethodID timestampInfoCtor = env->GetMethodID(timestampInfoClass, "<init>",
            "(Ljava/lang/String;[BLjava/lang/String;)V");
        jmethodID arrayListCtor = env->GetMethodID(arrayListClass, "<init>", "(I)V");
        jmethodID arrayListAdd = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

        if (contentsCtor == nullptr || signerInfoCtor == nullptr || timestampInfoCtor == nullptr ||
            arrayListCtor == nullptr || arrayListAdd == nullptr)
        {
            return nullptr;
        }

        const auto& signerInfos = contents.GetSignerInfos();
        jobject list = env->NewObject(arrayListClass, arrayListCtor, (jint)signerInfos.size());

        for (const auto& signer : signerInfos)
        {
            jbyteArray certBytes = nullptr;
            if (!signer.Certificate.empty())
            {
                certBytes = env->NewByteArray((jsize)signer.Certificate.size());
                env->SetByteArrayRegion(certBytes, 0, (jsize)signer.Certificate.size(),
                    reinterpret_cast<const jbyte*>(signer.Certificate.data()));
            }

            jstring subject = signer.Subject.empty() ? nullptr : stringToJstring(env, signer.Subject);
            jstring issuer = signer.Issuer.empty() ? nullptr : stringToJstring(env, signer.Issuer);
            jstring serial = signer.Serial.empty() ? nullptr : stringToJstring(env, signer.Serial);
            jstring signingTime = signer.SigningTime.has_value()
                ? stringToJstring(env, std::string(signer.SigningTime->ToStringW3C().GetString()))
                : nullptr;

            jobject signerInfo = env->NewObject(signerInfoClass, signerInfoCtor,
                certBytes, subject, issuer, serial, signingTime);
            env->CallBooleanMethod(list, arrayListAdd, signerInfo);

            env->DeleteLocalRef(signingTime);
            env->DeleteLocalRef(serial);
            env->DeleteLocalRef(issuer);
            env->DeleteLocalRef(subject);
            env->DeleteLocalRef(certBytes);
            env->DeleteLocalRef(signerInfo);
        }

        jobject timestampInfo = nullptr;
        PoDoFo::PdfSignatureTimestampInfo ts;
        if (contents.TryGetTimestampToken(ts))
        {
            jstring genTime = stringToJstring(env, std::string(ts.GenTime.ToStringW3C().GetString()));
            jbyteArray tsaCertBytes = nullptr;
            if (ts.TsaCertificate.has_value() && !ts.TsaCertificate->empty())
            {
                tsaCertBytes = env->NewByteArray((jsize)ts.TsaCertificate->size());
                env->SetByteArrayRegion(tsaCertBytes, 0, (jsize)ts.TsaCertificate->size(),
                    reinterpret_cast<const jbyte*>(ts.TsaCertificate->data()));
            }
            jstring tsaSubject = ts.TsaSubject.empty() ? nullptr : stringToJstring(env, ts.TsaSubject);

            timestampInfo = env->NewObject(timestampInfoClass, timestampInfoCtor,
                genTime, tsaCertBytes, tsaSubject);

            env->DeleteLocalRef(tsaSubject);
            env->DeleteLocalRef(tsaCertBytes);
            env->DeleteLocalRef(genTime);
        }

        jobject result = env->NewObject(contentsClass, contentsCtor, list, timestampInfo);

        env->DeleteLocalRef(timestampInfo);
        env->DeleteLocalRef(list);

        return result;
    }

    JNIEXPORT jboolean JNICALL Java_com_podofo_android_PdfSignature_nativeHasSignatureValue(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        return signature->HasSignatureValue() ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetFilter(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetFilter();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetSubFilter(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetSubFilter();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetType(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetType();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetName(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetSignerName();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetReason(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetSignatureReason();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetLocation(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetSignatureLocation();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetContactInfo(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetContactInfo();
        return value.has_value() ? stringToJstring(env, std::string(value->GetString())) : nullptr;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetSignDate(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetSignatureDate();
        return value.has_value()
            ? stringToJstring(env, std::string(value->ToStringW3C().GetString()))
            : nullptr;
    }

    JNIEXPORT jlongArray JNICALL Java_com_podofo_android_PdfSignature_nativeGetByteRange(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetByteRange();
        if (!value.has_value())
            return nullptr;

        jsize size = (jsize)value->GetSize();
        jlongArray result = env->NewLongArray(size);
        std::vector<jlong> temp(size);
        for (jsize i = 0; i < size; i++)
        {
            int64_t num;
            if (value->TryGetAtAs(i, num))
                temp[i] = (jlong)num;
            else
                temp[i] = 0;
        }
        env->SetLongArrayRegion(result, 0, size, temp.data());
        return result;
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfSignature_nativeGetPropBuild(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        auto value = signature->GetPropBuild();
        if (!value.has_value())
            return nullptr;

        try {
            PoDoFo::PdfVariant variant(*value);
            std::string str;
            variant.ToString(str, PoDoFo::PdfWriteFlags::None);
            return stringToJstring(env, str);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jbyteArray JNICALL Java_com_podofo_android_PdfSignature_nativeGetContents(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        PoDoFo::charbuff contents;
        if (!signature->TryGetContents(contents))
            return nullptr;

        jbyteArray result = env->NewByteArray((jsize)contents.size());
        env->SetByteArrayRegion(result, 0, (jsize)contents.size(),
            reinterpret_cast<const jbyte*>(contents.data()));
        return result;
    }

    JNIEXPORT jobject JNICALL Java_com_podofo_android_PdfSignature_nativeParseContents(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
        PoDoFo::PdfSignatureContents parsed;
        if (!signature->TryGetSignatureContents(parsed))
            return nullptr;

        try {
            if (!parsed.IsValid())
            {
                throwJavaException(env, "Failed to parse signature contents");
                return nullptr;
            }
            return createPdfSignatureContents(env, parsed);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return nullptr;
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfSignature_nativeVerifySignature(
        JNIEnv* env, jobject thiz, jlong handle, jbyteArray jSignedData) {

        try {
            auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);

            PoDoFo::PdfSignatureContents contents;
            if (!signature->TryGetSignatureContents(contents) || !contents.IsValid())
                return static_cast<jint>(PoDoFo::PdfSignatureVerifyStatus::CouldNotVerify);

            jsize length = env->GetArrayLength(jSignedData);
            jbyte* bytes = env->GetByteArrayElements(jSignedData, nullptr);
            if (bytes == nullptr)
                return static_cast<jint>(PoDoFo::PdfSignatureVerifyStatus::CouldNotVerify);

            PoDoFo::bufferview signedData(reinterpret_cast<const char*>(bytes), static_cast<size_t>(length));
            auto status = contents.VerifySignature(signedData);
            env->ReleaseByteArrayElements(jSignedData, bytes, JNI_ABORT);

            return static_cast<jint>(status);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return static_cast<jint>(PoDoFo::PdfSignatureVerifyStatus::CouldNotVerify);
        }
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfSignature_nativeVerifySignatureFromPath(
        JNIEnv* env, jobject thiz, jlong handle, jstring jDocumentPath) {

        try {
            auto* signature = reinterpret_cast<PoDoFo::PdfSignature*>(handle);
            std::string documentPath = jstringToString(env, jDocumentPath);

            PoDoFo::FileStreamDevice device(documentPath);
            return static_cast<jint>(signature->TryVerifySignature(device));
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return static_cast<jint>(PoDoFo::PdfSignatureVerifyStatus::CouldNotVerify);
        }
    }

    // ---- PdfAnnotation ----

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetAnnotationType(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* annot = reinterpret_cast<PoDoFo::PdfAnnotation*>(handle);
        switch (annot->GetType()) {
            case PoDoFo::PdfAnnotationType::Text: return stringToJstring(env, "Text");
            case PoDoFo::PdfAnnotationType::Link: return stringToJstring(env, "Link");
            case PoDoFo::PdfAnnotationType::FreeText: return stringToJstring(env, "FreeText");
            case PoDoFo::PdfAnnotationType::Line: return stringToJstring(env, "Line");
            case PoDoFo::PdfAnnotationType::Square: return stringToJstring(env, "Square");
            case PoDoFo::PdfAnnotationType::Circle: return stringToJstring(env, "Circle");
            case PoDoFo::PdfAnnotationType::Polygon: return stringToJstring(env, "Polygon");
            case PoDoFo::PdfAnnotationType::PolyLine: return stringToJstring(env, "PolyLine");
            case PoDoFo::PdfAnnotationType::Highlight: return stringToJstring(env, "Highlight");
            case PoDoFo::PdfAnnotationType::Underline: return stringToJstring(env, "Underline");
            case PoDoFo::PdfAnnotationType::Squiggly: return stringToJstring(env, "Squiggly");
            case PoDoFo::PdfAnnotationType::StrikeOut: return stringToJstring(env, "StrikeOut");
            case PoDoFo::PdfAnnotationType::Stamp: return stringToJstring(env, "Stamp");
            case PoDoFo::PdfAnnotationType::Caret: return stringToJstring(env, "Caret");
            case PoDoFo::PdfAnnotationType::Ink: return stringToJstring(env, "Ink");
            case PoDoFo::PdfAnnotationType::Popup: return stringToJstring(env, "Popup");
            case PoDoFo::PdfAnnotationType::FileAttachement: return stringToJstring(env, "FileAttachement");
            case PoDoFo::PdfAnnotationType::Sound: return stringToJstring(env, "Sound");
            case PoDoFo::PdfAnnotationType::Movie: return stringToJstring(env, "Movie");
            case PoDoFo::PdfAnnotationType::Widget: return stringToJstring(env, "Widget");
            case PoDoFo::PdfAnnotationType::Screen: return stringToJstring(env, "Screen");
            case PoDoFo::PdfAnnotationType::PrinterMark: return stringToJstring(env, "PrinterMark");
            case PoDoFo::PdfAnnotationType::TrapNet: return stringToJstring(env, "TrapNet");
            case PoDoFo::PdfAnnotationType::Watermark: return stringToJstring(env, "Watermark");
            case PoDoFo::PdfAnnotationType::Model3D: return stringToJstring(env, "Model3D");
            case PoDoFo::PdfAnnotationType::RichMedia: return stringToJstring(env, "RichMedia");
            case PoDoFo::PdfAnnotationType::WebMedia: return stringToJstring(env, "WebMedia");
            case PoDoFo::PdfAnnotationType::Redact: return stringToJstring(env, "Redact");
            case PoDoFo::PdfAnnotationType::Projection: return stringToJstring(env, "Projection");
            default: return stringToJstring(env, "Unknown");
        }
    }

    JNIEXPORT jdoubleArray JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetRect(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* annot = reinterpret_cast<PoDoFo::PdfAnnotation*>(handle);
        auto rect = annot->GetRect();
        jdoubleArray result = env->NewDoubleArray(4);
        jdouble values[4] = { rect.X, rect.Y, rect.Width, rect.Height };
        env->SetDoubleArrayRegion(result, 0, 4, values);
        return result;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfAnnotation_nativeSetRect(
        JNIEnv* env, jobject thiz, jlong handle, jdouble x, jdouble y, jdouble width, jdouble height) {

        auto* annot = reinterpret_cast<PoDoFo::PdfAnnotation*>(handle);
        annot->SetRect(PoDoFo::Rect(x, y, width, height));
    }

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfAnnotation_nativeGetContents(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* annot = reinterpret_cast<PoDoFo::PdfAnnotation*>(handle);
        auto contents = annot->GetContents();
        return contents.has_value() ? stringToJstring(env, std::string(contents.value().GetString())) : nullptr;
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfAnnotation_nativeSetContents(
        JNIEnv* env, jobject thiz, jlong handle, jstring jContents) {

        auto* annot = reinterpret_cast<PoDoFo::PdfAnnotation*>(handle);
        if (jContents == nullptr) {
            annot->SetContents(nullptr);
        } else {
            annot->SetContents(PoDoFo::PdfString(jstringToString(env, jContents)));
        }
    }

    // ---- PdfOutlineItem ----

    JNIEXPORT jstring JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetTitle(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
        return stringToJstring(env, std::string(item->GetTitle().GetString()));
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfOutlineItem_nativeSetTitle(
        JNIEnv* env, jobject thiz, jlong handle, jstring jTitle) {

        auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
        item->SetTitle(PoDoFo::PdfString(jstringToString(env, jTitle)));
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeCreateChild(
        JNIEnv* env, jobject thiz, jlong handle, jstring jTitle) {

        try {
            auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
            auto& child = item->CreateChild(PoDoFo::PdfString(jstringToString(env, jTitle)));
            return reinterpret_cast<jlong>(&child);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeCreateNext(
        JNIEnv* env, jobject thiz, jlong handle, jstring jTitle) {

        try {
            auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
            auto& next = item->CreateNext(PoDoFo::PdfString(jstringToString(env, jTitle)));
            return reinterpret_cast<jlong>(&next);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
            return 0;
        }
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetFirst(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
        auto* first = item->First();
        return reinterpret_cast<jlong>(first);
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetNext(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
        auto* next = item->Next();
        return reinterpret_cast<jlong>(next);
    }

    JNIEXPORT jlong JNICALL Java_com_podofo_android_PdfOutlineItem_nativeGetParent(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
        auto* parent = item->GetParentOutline();
        return reinterpret_cast<jlong>(parent);
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfOutlineItem_nativeSetDestinationToPage(
        JNIEnv* env, jobject thiz, jlong handle, jlong pageHandle) {

        try {
            auto* item = reinterpret_cast<PoDoFo::PdfOutlineItem*>(handle);
            auto* page = reinterpret_cast<PoDoFo::PdfPage*>(pageHandle);
            // PdfDestination's constructors are private (friend-only); the
            // public way to obtain one is PdfDocument::CreateDestination().
            auto dest = page->GetDocument().CreateDestination();
            dest->SetDestination(*page, PoDoFo::PdfDestinationFit::Fit);
            item->SetDestination(*dest);
        } catch (const std::exception& e) {
            throwJavaException(env, e.what());
        }
    }

    // ---- PdfImage ----

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfImage_nativeGetWidth(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* image = reinterpret_cast<PoDoFo::PdfImage*>(handle);
        return static_cast<jint>(image->GetWidth());
    }

    JNIEXPORT jint JNICALL Java_com_podofo_android_PdfImage_nativeGetHeight(
        JNIEnv* env, jobject thiz, jlong handle) {

        auto* image = reinterpret_cast<PoDoFo::PdfImage*>(handle);
        return static_cast<jint>(image->GetHeight());
    }

    JNIEXPORT void JNICALL Java_com_podofo_android_PdfImage_nativeDestroy(
        JNIEnv* env, jobject thiz, jlong handle) {

        if (handle) {
            delete reinterpret_cast<PoDoFo::PdfImage*>(handle);
        }
    }
}
