# Contributor: Alejandro Liu <alejandro_liu@hotmail.com>
# Maintainer: Alejandro Liu <alejandro_liu@hotmail.com>
pkgname=undup
pkgver=${TRAVIS_TAG:=$(git describe | tr - . | tr -dc .0-9 )}
pkgrel=0
pkgdesc="Track duplicate files and merge them as hardlinks"
url="https://github.com/TortugaLabs/undup"
arch="all"
license="GPL2"
depends=""
depends_dev=""
makedepends="$depends_dev"
install=""
subpackages=""
source=""

_builddir=
prepare() {
	local i
	cd "$_builddir"
	for i in $source; do
		case $i in
		*.patch) msg $i; patch -p1 -i "$srcdir"/$i || return 1;;
		esac
	done
}

build() {
	cd "$_builddir"
	make clean && make prod || return 1
}

package() {
	cd "$_builddir"
	mkdir -p "$pkgdir"/usr/bin
	cp undup "$pkgdir"/usr/bin
}

