// SPDX-FileCopyrightText: 2026 OpenCode Contributors
// SPDX-License-Identifier: LGPL-2.0-or-later OR MPL-2.0

#include <podofo/private/PdfDeclarationsPrivate.h>
#include "PdfSignatureContents.h"

#include <podofo/private/OpenSSLInternal.h>
#include <openssl/pkcs7.h>
#include <openssl/ts.h>
#include <openssl/x509.h>
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/asn1.h>
#include <openssl/objects.h>

#include <time.h>

#if _WIN32
#define timegm _mkgmtime
#endif

using namespace std;
using namespace PoDoFo;

static constexpr char SignatureTimeStampTokenOid[] = "1.2.840.113549.1.9.16.2.14";

static string getNameString(X509_NAME* name)
{
    if (name == nullptr)
        return { };

    BIO* bio = BIO_new(BIO_s_mem());
    if (bio == nullptr)
        return { };

    string ret;
    if (X509_NAME_print_ex(bio, name, 0, XN_FLAG_RFC2253) >= 0)
    {
        char* data;
        long len = BIO_get_mem_data(bio, &data);
        if (len > 0)
            ret.assign(data, (size_t)len);
    }

    BIO_free(bio);
    return ret;
}

static string getSerialString(const ASN1_INTEGER* serial)
{
    if (serial == nullptr)
        return { };

    BIGNUM* bn = ASN1_INTEGER_to_BN(serial, nullptr);
    if (bn == nullptr)
        return { };

    char* hex = BN_bn2hex(bn);
    string ret;
    if (hex != nullptr)
    {
        ret = hex;
        OPENSSL_free(hex);
    }

    BN_free(bn);
    return ret;
}

static bool tryConvertAsn1Time(const ASN1_TIME* time, PdfDate& date)
{
    if (time == nullptr)
        return false;

    struct tm tm;
    if (ASN1_TIME_to_tm(time, &tm) != 1)
        return false;

    time_t t = timegm(&tm);
    if (t == (time_t)-1)
        return false;

    date = PdfDate(chrono::seconds(t), nullptr);
    return true;
}

static int getSignatureTimeStampTokenNid()
{
    // The initializer of a function-local static is guaranteed by C++11 to run
    // exactly once, even under concurrent calls, so OBJ_create() can't race here
    static const int nid = OBJ_create(SignatureTimeStampTokenOid,
        "id-aa-signatureTimeStampToken",
        "id-aa-signatureTimeStampToken");

    return nid;
}

PdfSignatureContents::PdfSignatureContents()
    : m_valid(false)
{
}

PdfSignatureContents::PdfSignatureContents(const bufferview& contents)
    : PdfSignatureContents()
{
    (void)TryParse(contents);
}

bool PdfSignatureContents::TryParse(const bufferview& contents)
{
    m_valid = false;
    m_signerInfos.clear();
    m_timestampToken.clear();

    if (contents.empty())
        return false;

    BIO* bio = BIO_new_mem_buf(contents.data(), (int)contents.size());
    if (bio == nullptr)
        return false;

    PKCS7* p7 = d2i_PKCS7_bio(bio, nullptr);
    BIO_free(bio);

    if (p7 == nullptr)
        return false;

    STACK_OF(PKCS7_SIGNER_INFO)* signers = PKCS7_get_signer_info(p7);
    if (signers == nullptr)
    {
        PKCS7_free(p7);
        return false;
    }

    int signerCount = sk_PKCS7_SIGNER_INFO_num(signers);
    for (int i = 0; i < signerCount; i++)
    {
        PKCS7_SIGNER_INFO* si = sk_PKCS7_SIGNER_INFO_value(signers, i);
        if (si == nullptr)
            continue;

        PdfSignatureSignerInfo info;

        X509* signerCert = PKCS7_cert_from_signer_info(p7, si);
        if (signerCert != nullptr)
        {
            info.Subject = getNameString(X509_get_subject_name(signerCert));
            info.Issuer = getNameString(X509_get_issuer_name(signerCert));
            info.Serial = getSerialString(X509_get0_serialNumber(signerCert));

            int len = i2d_X509(signerCert, nullptr);
            if (len > 0)
            {
                info.Certificate.resize((size_t)len);
                unsigned char* p = reinterpret_cast<unsigned char*>(info.Certificate.data());
                i2d_X509(signerCert, &p);
            }
        }

        // Extract authenticated signingTime attribute
        ASN1_TYPE* signingTimeValue = PKCS7_get_signed_attribute(si, NID_pkcs9_signingTime);
        if (signingTimeValue != nullptr &&
            (signingTimeValue->type == V_ASN1_UTCTIME ||
             signingTimeValue->type == V_ASN1_GENERALIZEDTIME))
        {
            PdfDate date;
            if (tryConvertAsn1Time(signingTimeValue->value.utctime, date))
                info.SigningTime = date;
        }

        // Extract embedded timestamp token, if present
        int tsNid = getSignatureTimeStampTokenNid();
        if (tsNid != NID_undef)
        {
            ASN1_TYPE* tsTokenValue = PKCS7_get_attribute(si, tsNid);
            if (tsTokenValue != nullptr &&
                (tsTokenValue->type == V_ASN1_SEQUENCE ||
                 tsTokenValue->type == V_ASN1_OCTET_STRING))
            {
                const unsigned char* p = ASN1_STRING_get0_data(tsTokenValue->value.asn1_string);
                int len = ASN1_STRING_length(tsTokenValue->value.asn1_string);
                if (p != nullptr && len > 0)
                    m_timestampToken.assign((const char*)p, (size_t)len);
            }
        }

        m_signerInfos.push_back(std::move(info));
    }

    PKCS7_free(p7);

    m_valid = !m_signerInfos.empty();
    return m_valid;
}

bool PdfSignatureContents::TryGetTimestampToken(PdfSignatureTimestampInfo& info) const
{
    if (m_timestampToken.empty())
        return false;

    BIO* bio = BIO_new_mem_buf(m_timestampToken.data(), (int)m_timestampToken.size());
    if (bio == nullptr)
        return false;

    PKCS7* p7 = d2i_PKCS7_bio(bio, nullptr);
    BIO_free(bio);

    if (p7 == nullptr)
        return false;

    TS_TST_INFO* tstInfo = PKCS7_to_TS_TST_INFO(p7);
    if (tstInfo == nullptr)
    {
        PKCS7_free(p7);
        return false;
    }

    bool success = false;
    const ASN1_GENERALIZEDTIME* genTime = TS_TST_INFO_get_time(tstInfo);
    if (genTime != nullptr)
    {
        PdfDate date;
        if (tryConvertAsn1Time(genTime, date))
        {
            info.GenTime = date;
            success = true;
        }
    }

    if (success)
    {
        STACK_OF(X509)* tsCerts = PKCS7_get0_signers(p7, nullptr, 0);
        if (tsCerts != nullptr)
        {
            int certCount = sk_X509_num(tsCerts);
            if (certCount > 0)
            {
                X509* cert = sk_X509_value(tsCerts, 0);
                if (cert != nullptr)
                {
                    info.TsaSubject = getNameString(X509_get_subject_name(cert));

                    int len = i2d_X509(cert, nullptr);
                    if (len > 0)
                    {
                        charbuff certDer;
                        certDer.resize((size_t)len);
                        unsigned char* p = reinterpret_cast<unsigned char*>(certDer.data());
                        i2d_X509(cert, &p);
                        info.TsaCertificate = std::move(certDer);
                    }
                }
            }
            sk_X509_free(tsCerts);
        }

        // Try to extract TSA name from the TSTInfo if no cert was found
        if (info.TsaSubject.empty())
        {
            GENERAL_NAME* tsa = TS_TST_INFO_get_tsa(tstInfo);
            if (tsa != nullptr && tsa->type == GEN_DIRNAME)
                info.TsaSubject = getNameString(tsa->d.dirn);
        }
    }

    TS_TST_INFO_free(tstInfo);
    PKCS7_free(p7);
    return success;
}

