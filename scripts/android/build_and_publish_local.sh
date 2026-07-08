#!/bin/bash
# Builds the full Android AAR (all 4 ABIs) and publishes it to the local Maven
# repository (~/.m2/repository) — for local consumption/testing via
# mavenLocal(), no GitHub Packages/Maven Central credentials needed. Runs the
# same pipeline as the "Quick start" section in README_extended.md, then
# `./gradlew publishToMavenLocal`. Gradle writes ~/.m2/repository itself; the
# `mvn` CLI isn't actually invoked or required.
#
# Usage: scripts/android/build_and_publish_local.sh [--skip-deps] [NDK_DIR] [BUILD_ROOT] [VERSION]
#   --skip-deps  skip scripts/android/build_deps.sh — use on a repeat run
#                once dependencies are already built under BUILD_ROOT
#   NDK_DIR      defaults to $ANDROID_NDK_HOME
#   BUILD_ROOT   defaults to <repo>/build/android
#   VERSION      Maven version to publish under; defaults to whatever's in
#                src/android/gradle.properties (0.0.1-SNAPSHOT) if omitted
#
# Requires everything README_extended.md's Prerequisites section lists
# (Android NDK/SDK, CMake, JDK 17, autoconf/automake/libtool/pkg-config).

set -e

# Filters out --skip-deps regardless of its position among the positional
# args, rather than the "${@/"$arg"}" substitution trick used elsewhere in
# this repo (eg. scripts/android/podofo/build.sh) — that idiom replaces a
# matched argument with an empty string instead of actually removing it,
# which silently shifts every later positional argument by one slot unless
# the flag happens to be passed last.
SKIP_DEPS=0
POSITIONAL=()
for arg in "$@"; do
    if [ "$arg" == "--skip-deps" ]; then
        SKIP_DEPS=1
    else
        POSITIONAL+=("$arg")
    fi
done
set -- "${POSITIONAL[@]}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

NDK_DIR="${1:-$ANDROID_NDK_HOME}"
BUILD_ROOT="${2:-$REPO_ROOT/build/android}"
VERSION="${3:-}"

if [ -z "$NDK_DIR" ]; then
    echo "Error: no NDK_DIR given and ANDROID_NDK_HOME is not set" >&2
    exit 1
fi
BUILD_ROOT="$(mkdir -p "$BUILD_ROOT" && cd "$BUILD_ROOT" && pwd)"

if [ "$SKIP_DEPS" -eq 0 ]; then
    echo "=== 1/5: cross-compiling native dependencies (slow — pass --skip-deps to skip on a repeat run) ==="
    "$SCRIPT_DIR/build_deps.sh" "$NDK_DIR" "$BUILD_ROOT"
else
    echo "=== 1/5: skipping native dependencies (--skip-deps) ==="
fi

echo "=== 2/5: cross-compiling PoDoFo ==="
"$SCRIPT_DIR/podofo/build.sh" "$NDK_DIR" "$BUILD_ROOT/podofo" \
    "$BUILD_ROOT/freetype/install" "$BUILD_ROOT/libpng/install" \
    "$BUILD_ROOT/libxml2/install" "$BUILD_ROOT/openssl/install"

echo "=== 3/5: organizing static libs ==="
"$SCRIPT_DIR/podofo-aar/organize_libs.sh" \
    "$BUILD_ROOT/brotli/install" "$BUILD_ROOT/bzip2/install" \
    "$BUILD_ROOT/freetype/install" "$BUILD_ROOT/harfbuzz/install" \
    "$BUILD_ROOT/libpng/install" "$BUILD_ROOT/libxml2/install" \
    "$BUILD_ROOT/openssl/install" "$BUILD_ROOT/podofo/install" \
    "$BUILD_ROOT/zlib/install" "$BUILD_ROOT/aar"

echo "=== 4/5: linking JNI shared library ==="
"$SCRIPT_DIR/podofo-aar/build.sh" "$NDK_DIR" "$REPO_ROOT/src/wrapper" "$BUILD_ROOT/aar"

mkdir -p "$REPO_ROOT/src/android/src/main/jniLibs"
cp -r "$BUILD_ROOT/aar/build/jni/"* "$REPO_ROOT/src/android/src/main/jniLibs/"

echo "=== 5/5: publishing to Maven local ==="
# RELEASE_SIGNING_ENABLED=true in gradle.properties is meant for the Maven
# Central path: Gradle's signing plugin ties signing to the publication, not
# the destination repository, so publishToMavenLocal would otherwise also
# require a configured GPG key (same gotcha as the GitHub Packages CI step —
# see SUMMARIZE.md) — pointless for a local-only, never-published artifact.
GRADLE_ARGS=(-PRELEASE_SIGNING_ENABLED=false)
if [ -n "$VERSION" ]; then
    GRADLE_ARGS+=(-Pversion="$VERSION")
fi
(cd "$REPO_ROOT/src/android" && ./gradlew publishToMavenLocal "${GRADLE_ARGS[@]}")

echo
echo "Published to ~/.m2/repository/io/github/doko-demo-doa/podofo-android/"
echo "Consume from another Gradle project with:"
echo "  repositories { mavenLocal() }"
echo "  dependencies { implementation(\"io.github.doko-demo-doa:podofo-android:${VERSION:-<gradle.properties version>}\") }"
