# Mobile Changelog

Changes specific to the Android AAR and iOS XCFramework wrappers
(`src/wrapper/`, `src/android/`, `scripts/android/`, `scripts/ios/`) built on
top of PoDoFo core. See `CHANGELOG.md` for core library changes.

## Unreleased

### Signature verification consolidation
- Fixed a compile error (`redefinition of 'PdfSignatureVerifyStatus'`) introduced
  while syncing from upstream: the fork's `PdfSignatureContents::VerifySignature()`
  and upstream's newly-merged `PdfSignature::TryVerifySignature(InputStreamDevice&,
  PdfSignatureVerifyStatus&)` had independently defined an enum with the same name
  but different values
- **Breaking:** removed the fork's own verification path in favor of upstream's.
  Deleted `PdfSignatureContents::VerifySignature()`/`PdfSignatureContentsVerifyStatus`
  and the fork's single-arg `PdfSignature::TryVerifySignature(InputStreamDevice&)`
  convenience overload; all callers now use upstream's
  `TryVerifySignature(InputStreamDevice&, PdfSignatureVerifyStatus&)`, which also
  checks the CMS `SigningCertificateV2` attribute and streams the signed range
  instead of buffering it
  - Android: `PdfSignature.verifySignature(byte[])` removed. `VerifyStatus` enum
    changed from 3 values (`COULD_NOT_VERIFY`/`INVALID`/`VALID_NO_TRUST`) to 4
    (`INDETERMINATE`/`INVALID`/`CRYPTO_VERIFIED_PARTIAL_COVERAGE`/`CRYPTO_VERIFIED`)
  - iOS: `-[PoDoFoSignature verifySignatureWithData:]` removed. `PoDoFoSignatureVerifyStatus`
    changed the same way (`Indeterminate`/`Invalid`/`CryptoVerifiedPartialCoverage`/`CryptoVerified`)

### Build pipeline
- Fixed freetype builds on both platforms after GNU Savannah's download server
  went down (502/504): switched the source to GitHub's tag archive, which in
  turn needed two follow-on fixes since a GitHub tag snapshot isn't the same
  as Savannah's `make dist` tarball:
  - Generate the autotools `configure` script via `autogen.sh` (Savannah's
    tarball ships it pre-generated; GitHub's snapshot doesn't)
  - Fetch the `dlg` submodule's 3 files directly from its pinned commit
    (freetype always compiles `dlg` in, regardless of `FT_DEBUG_LOGGING`, and
    a GitHub tag archive doesn't carry submodule content)
- Added a `download_verified()` helper (`scripts/{ios,android}/common.sh`) used
  by every dependency's `build.sh`: treats HTTP errors as failures and checks
  gzip integrity before and after download, instead of silently caching a
  truncated/error-page response as if it were the real archive
- Fixed two long-standing cache-miss bugs found while wiring up
  `download_verified()`: Android brotli's existence check used the wrong
  filename, and Android openssl's was a mangled/always-false string

## Android AAR + iOS XCFramework builds

Initial and ongoing build-out of both native wrappers, roughly in build order:

- Android AAR build support and Maven Central publishing
- iOS XCFramework build support, with API parity to the Android wrapper
  (`PoDoFoDocument`, `PoDoFoPage`, `PoDoFoPainter`, `PoDoFoSignature`, ...)
- Unified the Android/iOS GitHub Actions release process so both workflows can
  publish to the same tagged GitHub Release without racing to create it
- Cryptographic signature verification exposed to both platforms
- Document encryption status check (`isEncrypted`)
- Page creation/manipulation methods on both platforms
- Signature metadata accessors (reason, location, contact info, signing time, ...)
- Font loading from an in-memory buffer
- Visible (stamped) text/image signature support
- ccache cache-key improvements for both native dependency pipelines
- Release notes generation improvements (SHA256 checksums, per-platform install snippets)
