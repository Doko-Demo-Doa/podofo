#import "PoDoFoWrapper.h"
#import "PoDoFoInternal.h"

#include <podofo/podofo.h>

using namespace PoDoFo;

static std::vector<std::string> PoDoFoStringVectorFromArray(NSArray<NSString *> * _Nullable array)
{
    std::vector<std::string> result;
    if (array == nil)
        return result;
    result.reserve(array.count);
    for (NSString *item in array) {
        result.push_back(item.UTF8String);
    }
    return result;
}

static std::optional<ValidationData> PoDoFoValidationDataFromArrays(NSArray<NSString *> * _Nullable certificates,
                                                                     NSArray<NSString *> * _Nullable crls,
                                                                     NSArray<NSString *> * _Nullable ocsps)
{
    if (certificates == nil && crls == nil && ocsps == nil)
        return std::nullopt;

    ValidationData data;
    for (const auto &cert : PoDoFoStringVectorFromArray(certificates))
        data.addCertificate(cert);
    for (const auto &crl : PoDoFoStringVectorFromArray(crls))
        data.addCRL(crl);
    for (const auto &ocsp : PoDoFoStringVectorFromArray(ocsps))
        data.addOCSP(ocsp);
    return data;
}

@implementation PoDoFoWrapper {
    std::unique_ptr<PdfRemoteSignDocumentSession> _session;
}

- (nullable instancetype)initWithConformanceLevel:(NSString *)conformanceLevel
                                     hashAlgorithm:(NSString *)hashAlgorithm
                                         inputPath:(NSString *)inputPath
                                        outputPath:(NSString *)outputPath
                                       certificate:(NSString *)certificate
                                 chainCertificates:(NSArray<NSString *> *)chainCertificates
                                             error:(NSError **)error
{
    self = [super init];
    if (self) {
        try {
            _session = std::make_unique<PdfRemoteSignDocumentSession>(
                conformanceLevel.UTF8String,
                hashAlgorithm.UTF8String,
                inputPath.UTF8String,
                outputPath.UTF8String,
                certificate.UTF8String,
                PoDoFoStringVectorFromArray(chainCertificates),
                std::nullopt);
        } catch (const std::exception &e) {
            PoDoFoSetErrorFromException(error, e);
            return nil;
        }
    }
    return self;
}

- (BOOL)isLoaded
{
    return _session != nullptr;
}

- (void)printState
{
    if (_session)
        _session->printState();
}

- (void)close
{
    _session.reset();
}

- (void)dealloc
{
    [self close];
}

- (BOOL)checkLoaded:(NSError **)error
{
    if (_session == nullptr) {
        PoDoFoSetError(error, PoDoFoErrorInvalidState, @"Session not initialized");
        return NO;
    }
    return YES;
}

- (nullable NSString *)calculateHashAndReturnError:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string hash = _session->beginSigning();
        return hash.empty() ? nil : [NSString stringWithUTF8String:hash.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (BOOL)finalizeSigningWithSignedHash:(NSString *)signedHash
                                   tsr:(nullable NSString *)tsr
                validationCertificates:(nullable NSArray<NSString *> *)certificates
                        validationCRLs:(nullable NSArray<NSString *> *)crls
                       validationOCSPs:(nullable NSArray<NSString *> *)ocsps
                                 error:(NSError **)error
{
    if (![self checkLoaded:error])
        return NO;
    if (signedHash == nil) {
        PoDoFoSetError(error, PoDoFoErrorInvalidArgument, @"Cannot finalize with nil signed hash");
        return NO;
    }
    try {
        _session->finishSigning(signedHash.UTF8String,
            tsr != nil ? tsr.UTF8String : "",
            PoDoFoValidationDataFromArrays(certificates, crls, ocsps));
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

- (nullable NSString *)beginSigningLTAAndReturnError:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string hash = _session->beginSigningLTA();
        return hash.empty() ? nil : [NSString stringWithUTF8String:hash.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (BOOL)finishSigningLTAWithTSR:(NSString *)tsr
          validationCertificates:(nullable NSArray<NSString *> *)certificates
                  validationCRLs:(nullable NSArray<NSString *> *)crls
                 validationOCSPs:(nullable NSArray<NSString *> *)ocsps
                           error:(NSError **)error
{
    if (![self checkLoaded:error])
        return NO;
    try {
        _session->finishSigningLTA(tsr != nil ? tsr.UTF8String : "",
            PoDoFoValidationDataFromArrays(certificates, crls, ocsps));
        return YES;
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return NO;
    }
}

- (nullable NSString *)crlFromCertificate:(NSString *)base64Cert error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string url = _session->getCrlFromCertificate(base64Cert.UTF8String);
        return url.empty() ? nil : [NSString stringWithUTF8String:url.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable NSString *)signerCertificateFromTSR:(NSString *)base64Tsr error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string cert = _session->extractSignerCertFromTSR(base64Tsr.UTF8String);
        return cert.empty() ? nil : [NSString stringWithUTF8String:cert.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable NSString *)issuerCertificateFromTSR:(NSString *)base64Tsr error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string cert = _session->extractIssuerCertFromTSR(base64Tsr.UTF8String);
        return cert.empty() ? nil : [NSString stringWithUTF8String:cert.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable NSString *)ocspFromCertificate:(NSString *)base64Cert
                          issuerCertificate:(NSString *)base64IssuerCert
                                      error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string url = _session->getOCSPFromCertificate(base64Cert.UTF8String, base64IssuerCert.UTF8String);
        return url.empty() ? nil : [NSString stringWithUTF8String:url.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable NSString *)buildOCSPRequestFromCertificate:(NSString *)base64Cert
                                      issuerCertificate:(NSString *)base64IssuerCert
                                                  error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string request = _session->buildOCSPRequestFromCertificates(base64Cert.UTF8String, base64IssuerCert.UTF8String);
        return request.empty() ? nil : [NSString stringWithUTF8String:request.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

- (nullable NSString *)certificateIssuerURLFromCertificate:(NSString *)base64Cert error:(NSError **)error
{
    if (![self checkLoaded:error])
        return nil;
    try {
        std::string url = _session->getCertificateIssuerUrlFromCertificate(base64Cert.UTF8String);
        return url.empty() ? nil : [NSString stringWithUTF8String:url.c_str()];
    } catch (const std::exception &e) {
        PoDoFoSetErrorFromException(error, e);
        return nil;
    }
}

@end
