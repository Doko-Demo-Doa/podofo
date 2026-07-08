// SPDX-FileCopyrightText: 2026 OpenCode Contributors
// SPDX-License-Identifier: LGPL-2.0-or-later OR MPL-2.0

#ifndef PDF_SIGNATURE_CONTENTS_H
#define PDF_SIGNATURE_CONTENTS_H

#include "PdfDeclarations.h"
#include "PdfDate.h"

namespace PoDoFo {

/// Information about a CMS/PKCS7 signer extracted from a signature /Contents blob.
struct PODOFO_API PdfSignatureSignerInfo
{
    /// DER-encoded signer certificate.
    charbuff Certificate;

    /// X.509 subject name (RFC 2253 style).
    std::string Subject;

    /// X.509 issuer name (RFC 2253 style).
    std::string Issuer;

    /// Serial number as a hexadecimal string.
    std::string Serial;

    /// CMS authenticated signingTime attribute, if present.
    nullable<PdfDate> SigningTime;
};

/// Information about an embedded RFC3161 timestamp token.
struct PODOFO_API PdfSignatureTimestampInfo
{
    /// genTime from the TSTInfo.
    PdfDate GenTime;

    /// DER-encoded TSA certificate, if embedded in the token.
    nullable<charbuff> TsaCertificate;

    /// TSA subject name, if available.
    std::string TsaSubject;
};

/// Parser for the raw CMS/PKCS7 data stored in a PDF signature /Contents entry.
/// This class extracts signer information and embedded RFC3161 timestamp tokens
/// without performing cryptographic verification.
class PODOFO_API PdfSignatureContents final
{
public:
    PdfSignatureContents();

    /// Parse the given raw signature contents.
    /// @param contents raw bytes from the PDF /Contents entry
    explicit PdfSignatureContents(const bufferview& contents);

    PdfSignatureContents(const PdfSignatureContents&) = delete;
    PdfSignatureContents(PdfSignatureContents&&) = default;
    PdfSignatureContents& operator=(const PdfSignatureContents&) = delete;
    PdfSignatureContents& operator=(PdfSignatureContents&&) = default;

    /// Parse the given raw signature contents.
    /// @param contents raw bytes from the PDF /Contents entry
    /// @returns true if the contents could be parsed as CMS/PKCS7
    bool TryParse(const bufferview& contents);

    /// Returns true if the contents were parsed successfully.
    bool IsValid() const { return m_valid; }

    /// Get all signer infos found in the CMS message.
    const std::vector<PdfSignatureSignerInfo>& GetSignerInfos() const { return m_signerInfos; }

    /// Try to extract an embedded RFC3161 timestamp token
    /// (signatureTimeStampToken unsigned attribute).
    /// @param info output structure that will receive the timestamp info
    /// @returns true if a timestamp token was found and parsed
    bool TryGetTimestampToken(PdfSignatureTimestampInfo& info) const;

private:
    bool m_valid;
    std::vector<PdfSignatureSignerInfo> m_signerInfos;
    charbuff m_timestampToken;
};

}

#endif // PDF_SIGNATURE_CONTENTS_H
