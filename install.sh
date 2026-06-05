#!/usr/bin/env bash
set -e

PREFIX="${PREFIX:-/usr/local}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if command -v gcc >/dev/null 2>&1; then
    CC=gcc
elif command -v clang >/dev/null 2>&1; then
    CC=clang
else
    echo "Error: no C compiler found (gcc or clang). Install one and try again." >&2
    exit 1
fi

echo "Building fasterfetch..."
make clean
make CC="$CC"

echo "Installing to $PREFIX ..."
make install PREFIX="$PREFIX" DESTDIR="$DESTDIR" CC="$CC"

echo ""
echo "Installed: $PREFIX/bin/fasterfetch"
echo ""
echo "To use your own ASCII art:"
echo "  mkdir -p ~/.config/fasterfetch"
echo "  cp /path/to/your/ascii.txt ~/.config/fasterfetch/ascii.txt"
echo ""
echo "Run 'fasterfetch -h' for all options."
