# Maintainer: Your Name <youremail@domain.com>
pkgname=yawn
pkgver=0.1
pkgrel=1
pkgdesc="A simple window manager"
arch=('i686' 'x86_64')
url="neos300.com"
license=('LGPL')
groups=()
depends=('dmenu' 'xorg-server' 'xorg-server-common' 'xorg-xinit')
makedepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=(http://neos300.com/$pkgname-$pkgver.tar.gz)
noextract=()
md5sums=('4da21ec2ae95a8a29026950dfeef7055')

build() {
  cd "$srcdir/$pkgname-$pkgver"

  ./configure --prefix=/usr
  make
}

package() {
  cd "$srcdir/$pkgname-$pkgver"

  make DESTDIR="$pkgdir/" install
}
