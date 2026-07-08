#!/bin/bash
# Cross-compiles every native dependency PoDoFo needs on iOS, for all 3 build
# slices (ios-arm64, ios-arm64-simulator, ios-x86_64-simulator), using the
# per-dependency scripts in this directory. Run this once (it's slow —
# several native libraries built from source, three times each) before
# scripts/ios/podofo/build.sh.
#
# Usage: scripts/ios/build_deps.sh [BUILD_ROOT]
#   BUILD_ROOT defaults to <repo>/build/ios

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

BUILD_ROOT="${1:-$SCRIPT_DIR/../../build/ios}"
BUILD_ROOT="$(mkdir -p "$BUILD_ROOT" && cd "$BUILD_ROOT" && pwd)"

# Same pinned versions as the Android pipeline (scripts/android/build_deps.sh)
# — kept identical across both platforms so behavior doesn't silently diverge
# between them.
BROTLI_VERSION="${BROTLI_VERSION:-1.0.9}"
BZIP2_VERSION="${BZIP2_VERSION:-1.0.8}"
FREETYPE_VERSION="${FREETYPE_VERSION:-2.13.2}"
LIBPNG_VERSION="${LIBPNG_VERSION:-1.6.43}"
LIBXML2_VERSION="${LIBXML2_VERSION:-2.12.3}"
OPENSSL_VERSION="${OPENSSL_VERSION:-3.2.4}"
HARFBUZZ_VERSION="${HARFBUZZ_VERSION:-11.2.1}"
ZLIB_VERSION="${ZLIB_VERSION:-1.2.13}"

echo "=== zlib $ZLIB_VERSION ==="
"$SCRIPT_DIR/zlib/build.sh" "$BUILD_ROOT/zlib" "$ZLIB_VERSION"

echo "=== brotli $BROTLI_VERSION ==="
"$SCRIPT_DIR/brotli/build.sh" "$BUILD_ROOT/brotli" "$BROTLI_VERSION"

echo "=== bzip2 $BZIP2_VERSION ==="
"$SCRIPT_DIR/bzip2/build.sh" "$BUILD_ROOT/bzip2" "$BZIP2_VERSION"

echo "=== freetype $FREETYPE_VERSION (needs brotli+bzip2) ==="
"$SCRIPT_DIR/freetype/build.sh" "$BUILD_ROOT/freetype" "$FREETYPE_VERSION" \
    "$BUILD_ROOT/bzip2/install" "$BUILD_ROOT/brotli/install"

echo "=== libpng $LIBPNG_VERSION (needs zlib) ==="
"$SCRIPT_DIR/libpng/build.sh" "$BUILD_ROOT/libpng" "$LIBPNG_VERSION" "$BUILD_ROOT/zlib/install"

echo "=== libxml2 $LIBXML2_VERSION ==="
"$SCRIPT_DIR/libxml2/build.sh" "$BUILD_ROOT/libxml2" "$LIBXML2_VERSION"

echo "=== openssl $OPENSSL_VERSION ==="
"$SCRIPT_DIR/openssl/build.sh" "$BUILD_ROOT/openssl" "$OPENSSL_VERSION"

echo "=== harfbuzz $HARFBUZZ_VERSION ==="
"$SCRIPT_DIR/harfbuzz/build.sh" "$BUILD_ROOT/harfbuzz" "$HARFBUZZ_VERSION"

echo "All iOS dependencies built under $BUILD_ROOT"
