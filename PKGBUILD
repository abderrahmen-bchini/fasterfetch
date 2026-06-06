# Maintainer: Abderrahmen Bchini <youssefothman207@gmail.com>
pkgname=fasterfetch
pkgver=1.0.0
pkgrel=1
pkgdesc="A retro fetch tool that reads system info directly from kernel interfaces — no subprocesses, no bloat"
arch=('x86_64' 'aarch64' 'armv7h')
url="https://github.com/abderrahmen-bchini/fasterfetch"
license=('custom')
makedepends=('gcc')
source=("$pkgname-$pkgver.tar.gz::$url/archive/v$pkgver/$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$pkgname-$pkgver"
    gcc -O2 -Wall -Wextra fasterfetch.c -o fasterfetch
}

package() {
    cd "$pkgname-$pkgver"
    install -Dm755 fasterfetch         "$pkgdir/usr/bin/fasterfetch"
    install -Dm644 fasterfetch.1       "$pkgdir/usr/share/man/man1/fasterfetch.1"
    if [ -f ascii.txt ]; then
        install -Dm644 ascii.txt       "$pkgdir/usr/share/fasterfetch/ascii.txt"
    fi
}
