#!/bin/bash
# Shared helpers sourced by every scripts/android/<dep>/build.sh. Not meant to be
# executed directly.
#
# The upstream reference this pipeline is ported from
# (eu-digital-identity-wallet/eudi-lib-podofo) only ever runs its Android build
# scripts on Ubuntu CI, so it hardcodes the NDK's "linux-x86_64" prebuilt
# toolchain directory and calls the Linux-only `nproc`. This repo also builds
# locally on macOS, whose NDK ships a "darwin-x86_64" toolchain and has no
# `nproc`, so every adapted script sources this file instead of hardcoding
# either.

case "$(uname -s)" in
    Darwin) HOST_TAG="darwin-x86_64"; JNI_PLATFORM_DIR="darwin" ;;
    Linux)  HOST_TAG="linux-x86_64";  JNI_PLATFORM_DIR="linux" ;;
    *)
        echo "Error: unsupported host OS '$(uname -s)' for Android cross-compilation" >&2
        exit 1
        ;;
esac

# libxml2's autogen.sh (needed since we build from a git tag archive, which
# ships no pre-generated configure script) requires GNU libtool/libtoolize.
# macOS ships an unrelated Apple `libtool`; Homebrew's libtool installs the
# GNU one under a "g" prefix specifically to avoid clashing with it, and
# recommends prepending its gnubin dir to PATH to get the normal names back.
if [ "$(uname -s)" = "Darwin" ] && [ -d "/opt/homebrew/opt/libtool/libexec/gnubin" ]; then
    PATH="/opt/homebrew/opt/libtool/libexec/gnubin:$PATH"
elif [ "$(uname -s)" = "Darwin" ] && [ -d "/usr/local/opt/libtool/libexec/gnubin" ]; then
    PATH="/usr/local/opt/libtool/libexec/gnubin:$PATH"
fi

# JAVA_HOME/include/<platform> holds the platform-specific jni_md.h; the plain
# "linux" the reference always assumes doesn't exist on a macOS JDK. Only the
# podofo-aar script (which compiles the JNI glue) actually needs this resolved;
# it's looked up here (best-effort) so every script shares one detection path,
# but only that script's own check() enforces it's non-empty.
if [ -z "$JAVA_HOME" ]; then
    if [ "$(uname -s)" = "Darwin" ] && command -v /usr/libexec/java_home >/dev/null 2>&1; then
        JAVA_HOME="$(/usr/libexec/java_home 2>/dev/null)"
    elif command -v javac >/dev/null 2>&1; then
        JAVA_HOME="$(cd "$(dirname "$(readlink -f "$(command -v javac)")")/.." && pwd)"
    fi
fi

if command -v nproc >/dev/null 2>&1; then
    NPROC="$(nproc)"
else
    NPROC="$(sysctl -n hw.ncpu)"
fi

# Several vendored dependency CMakeLists.txt files (eg. brotli, harfbuzz) declare
# an old cmake_minimum_required() that CMake >= 4.0 refuses to configure at all
# ("Compatibility with CMake < 3.5 has been removed"). This env var is CMake's
# own escape hatch for exactly this, and applies to every `cmake` invocation
# below without having to patch each dependency's CMakeLists.txt individually.
export CMAKE_POLICY_VERSION_MINIMUM=3.5

# Portable in-place sed: BSD/macOS sed requires an (empty) backup suffix argument
# after -i, GNU/Linux sed treats a following argument as a suffix if given at all.
sed_i() {
    if [ "$(uname -s)" = "Darwin" ]; then
        sed -i '' "$@"
    else
        sed -i "$@"
    fi
}
