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

# Wrap the NDK's clang/clang++ with ccache when it's installed (eg. via
# hendrikmuhs/ccache-action in CI), so repeated cross-compiles of unchanged
# sources can skip recompilation. This matters more than usual here: PoDoFo's
# build.sh does a fresh `rsync` of the source tree before each build, which
# resets file mtimes and defeats make/ninja's own incremental rebuild even
# when the content is unchanged. No-op (returns the path unchanged) if ccache
# isn't on PATH. Used for Makefile/autotools-based builds, which just need
# "ccache <path>" prefixed onto $CC/$CXX; CMake-based builds need a different
# mechanism (CMAKE_<LANG>_COMPILER_LAUNCHER, or NDK's own ANDROID_CCACHE) since
# CMAKE_C_COMPILER must be a single executable path, not "ccache <path>".
if command -v ccache >/dev/null 2>&1; then
    CCACHE_BIN="ccache"
else
    CCACHE_BIN=""
fi

with_ccache() {
    if [ -n "$CCACHE_BIN" ]; then
        echo "$CCACHE_BIN $1"
    else
        echo "$1"
    fi
}

# Downloads a .tar.gz to $2 if missing, verifying gzip integrity either way.
# Plain `curl -L` silently saves a short error-page response as if it were
# the real file, and a corrupt cached download poisons every future run.
download_verified() {
    local url="$1"
    local dest="$2"

    if [ -f "$dest" ] && ! gzip -t "$dest" >/dev/null 2>&1; then
        echo "Warning: $dest exists but is not a valid gzip archive (a previous download likely failed) -- re-downloading" >&2
        rm -f "$dest"
    fi

    if [ ! -f "$dest" ]; then
        curl -fL --retry 3 --retry-delay 5 --retry-all-errors -o "$dest" "$url"
        if ! gzip -t "$dest" >/dev/null 2>&1; then
            rm -f "$dest"
            echo "Error: downloaded $dest but it is not a valid gzip archive" >&2
            exit 1
        fi
    fi
}
