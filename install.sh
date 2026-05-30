#!/usr/bin/env bash
set -e

PREFIX="${PREFIX:-/usr/local}"
BINDIR="$PREFIX/bin"
DATADIR="$PREFIX/share/fasterfetch"
MANDIR="$PREFIX/share/man/man1"

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
$CC -O2 -Wall -Wextra "$SCRIPT_DIR/fasterfetch.c" -o "$SCRIPT_DIR/fasterfetch"

echo "Installing to $PREFIX ..."
install -Dm755 "$SCRIPT_DIR/fasterfetch" "$BINDIR/fasterfetch"
if [ -f "$SCRIPT_DIR/ascii.txt" ]; then
    install -Dm644 "$SCRIPT_DIR/ascii.txt" "$DATADIR/ascii.txt"
fi
if [ -f "$SCRIPT_DIR/fasterfetch.1" ]; then
    install -Dm644 "$SCRIPT_DIR/fasterfetch.1" "$MANDIR/fasterfetch.1"
fi

echo ""
echo "Installed: $BINDIR/fasterfetch"
echo ""
echo "To use your own ASCII art:"
echo "  mkdir -p ~/.config/fasterfetch"
echo "  cp /path/to/your/ascii.txt ~/.config/fasterfetch/ascii.txt"
echo ""
echo "Run 'fasterfetch -h' for all options."
