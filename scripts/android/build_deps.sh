#!/bin/bash
# Cross-compiles every native dependency PoDoFo needs on Android, for all 4 ABIs,
# using the per-dependency scripts in this directory. Run this once (it's slow —
# several native libraries built from source, four times each) before
# scripts/android/podofo/build.sh.
#
# Usage: scripts/android/build_deps.sh [NDK_DIR] [BUILD_ROOT]
#   NDK_DIR    defaults to $ANDROID_NDK_HOME
#   BUILD_ROOT defaults to <repo>/build/android

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

NDK_DIR="${1:-$ANDROID_NDK_HOME}"
BUILD_ROOT="${2:-$SCRIPT_DIR/../../build/android}"

if [ -z "$NDK_DIR" ]; then
    echo "Error: no NDK_DIR given and ANDROID_NDK_HOME is not set" >&2
    exit 1
fi
BUILD_ROOT="$(mkdir -p "$BUILD_ROOT" && cd "$BUILD_ROOT" && pwd)"

# Versions pinned to what eu-digital-identity-wallet/eudi-lib-podofo's CI validates
BROTLI_VERSION="${BROTLI_VERSION:-1.0.9}"
BZIP2_VERSION="${BZIP2_VERSION:-1.0.8}"
FREETYPE_VERSION="${FREETYPE_VERSION:-2.13.2}"
LIBPNG_VERSION="${LIBPNG_VERSION:-1.6.43}"
LIBXML2_VERSION="${LIBXML2_VERSION:-2.12.3}"
OPENSSL_VERSION="${OPENSSL_VERSION:-3.2.4}"
HARFBUZZ_VERSION="${HARFBUZZ_VERSION:-11.2.1}"
ZLIB_VERSION="${ZLIB_VERSION:-1.2.13}"

echo "=== brotli $BROTLI_VERSION ==="
"$SCRIPT_DIR/brotli/build.sh" "$NDK_DIR" "$BUILD_ROOT/brotli" "$BROTLI_VERSION"

echo "=== bzip2 $BZIP2_VERSION ==="
"$SCRIPT_DIR/bzip2/build.sh" "$NDK_DIR" "$BUILD_ROOT/bzip2" "$BZIP2_VERSION"

echo "=== freetype $FREETYPE_VERSION (needs brotli+bzip2) ==="
"$SCRIPT_DIR/freetype/build.sh" "$NDK_DIR" "$BUILD_ROOT/freetype" "$FREETYPE_VERSION" \
    "$BUILD_ROOT/bzip2/install" "$BUILD_ROOT/brotli/install"

echo "=== libpng $LIBPNG_VERSION ==="
"$SCRIPT_DIR/libpng/build.sh" "$NDK_DIR" "$BUILD_ROOT/libpng" "$LIBPNG_VERSION"

echo "=== libxml2 $LIBXML2_VERSION ==="
"$SCRIPT_DIR/libxml2/build.sh" "$NDK_DIR" "$BUILD_ROOT/libxml2" "$LIBXML2_VERSION"

echo "=== openssl $OPENSSL_VERSION ==="
"$SCRIPT_DIR/openssl/build.sh" "$NDK_DIR" "$BUILD_ROOT/openssl" "$OPENSSL_VERSION"

echo "=== harfbuzz $HARFBUZZ_VERSION ==="
"$SCRIPT_DIR/harfbuzz/build.sh" "$NDK_DIR" "$BUILD_ROOT/harfbuzz" "$HARFBUZZ_VERSION"

echo "=== zlib $ZLIB_VERSION ==="
"$SCRIPT_DIR/zlib/build.sh" "$NDK_DIR" "$BUILD_ROOT/zlib" "$ZLIB_VERSION"

echo "All Android dependencies built under $BUILD_ROOT"
