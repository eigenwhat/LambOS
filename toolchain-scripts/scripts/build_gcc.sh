#!/usr/bin/env bash

source ./getopt.bash

OPTS=`getopt -o v:p:t: --long version:,prefix:,target: -n 'parse-options' -- "$@"`

if [ $? != 0 ]; then
    echo "Failed parsing options." >&2
    exit 1
fi

echo "build_gcc.sh: $OPTS"
eval set -- "$OPTS"

GCC_VERSION=7.3.0
PREFIX=$HOME/.cross
TARGET=i586-elf
PATH="$PREFIX/bin:$PATH"

VERSION_PROVIDED=0

while true; do
    case "$1" in
        -v | --version ) GCC_VERSION="$2"; VERSION_PROVIDED=1; shift; shift;;
        -p | --prefix ) PREFIX="$2"; shift; shift;;
        -t | --target ) TARGET="$2"; shift; shift;;
        -- ) shift; break;;
        * ) break;;
    esac
done

if [ $VERSION_PROVIDED -eq 0 ]; then
	echo "No GCC version provided. Defaulting to gcc-${GCC_VERSION}..."
fi

if [ ! -f gcc-${GCC_VERSION}.tar.* ]
then
	echo "No GCC tarball located. Downloading now..."

	GCC_TARBALL_BASE=gcc-${GCC_VERSION}.tar.
    for EX in xz bz2 gz
    do
        wget --spider https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/${GCC_TARBALL_BASE}${EX}
        if [ $? == 0 ]; then
            GCC_TARBALL=${GCC_TARBALL_BASE}${EX}
            break;
        fi
    done

    if [ -z "$GCC_TARBALL" ]; then
        echo "Unable to find tarball for GCC-${GCC_VERSION}. Exiting..."
        exit -1
    fi

	wget https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/${GCC_TARBALL}
	if [ $? != 0 ]; then
	    echo "Error downloading ${GCC_TARBALL}. Exiting..."
	    exit -1
	fi
fi

if [ ! -d gcc-${GCC_VERSION} ]
then
	tar xf gcc-${GCC_VERSION}.tar.*
	if [ $? != 0 ]; then
	    echo "Error unpacking gcc-${GCC_VERSION}. Exiting..."
	    exit -1
	fi
fi

if [ -z "${MAKE}" ]; then
    echo "MAKE not defined. Setting to gmake..."
    MAKE=gmake
fi

rm -rf build-gcc
mkdir build-gcc
cd build-gcc

../gcc-${GCC_VERSION}/configure --target=$TARGET --prefix="$PREFIX" --with-gmp=/usr/local --with-mpfr=/usr/local --with-mpc=/usr/local --disable-nls --enable-languages=c,c++ --without-headers
${MAKE} -j7 all-gcc
${MAKE} -j7 all-target-libgcc
${MAKE} install-gcc
${MAKE} install-target-libgcc

