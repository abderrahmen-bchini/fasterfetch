#!/usr/bin/env bash
set -e

PREFIX="${PREFIX:-/usr/local}"

if [ "$PREFIX" = "/usr/local" ] && [ "$EUID" -ne 0 ]; then
    echo "Error: Please run as root (use sudo) to uninstall from $PREFIX"
    exit 1
fi

echo "Uninstalling fasterfetch from $PREFIX ..."

if [ -f "Makefile" ]; then
    make uninstall PREFIX="$PREFIX"
else
    rm -f "$PREFIX/bin/fasterfetch"
    rm -f "$PREFIX/share/man/man1/fasterfetch.1"
    rm -f "$PREFIX/share/fasterfetch/ascii.txt"
    rmdir "$PREFIX/share/fasterfetch" 2>/dev/null || true
fi

echo "Successfully uninstalled fasterfetch."
