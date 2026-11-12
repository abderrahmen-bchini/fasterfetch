# Maintainer: Abderrahmen Bchini <youssefothman207@gmail.com>
pkgname=fasterfetch
pkgver=1.0.0
pkgrel=1
pkgdesc="A retro fetch tool that reads system info directly from kernel interfaces — no subprocesses, no bloat"
arch=('x86_64' 'aarch64' 'armv7h')
url="https://github.com/abderrahmen-bchini/fasterfetch"
license=('MIT')
makedepends=('gcc' 'make')
source=("$pkgname-$pkgver.tar.gz::$url/archive/v$pkgver/$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$pkgname-$pkgver"
    make CC=gcc
}

package() {
    cd "$pkgname-$pkgver"
    make install DESTDIR="$pkgdir" PREFIX=/usr
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
