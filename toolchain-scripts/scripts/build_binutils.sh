#!/usr/bin/env bash

source ./getopt.bash

OPTS=`getopt -o v:p:t: --long version:,prefix:,target: -n 'parse-options' -- "$@"`

if [ $? != 0 ]; then
    echo "Failed parsing options." >&2
    exit 1
fi

echo "build_binutils.sh: $OPTS"
eval set -- "$OPTS"

BINUTILS_VERSION=2.30
PREFIX=$HOME/.cross
TARGET=i586-elf
PATH="$PREFIX/bin:$PATH"

VERSION_PROVIDED=0

while true; do
    case "$1" in
        -v | --version ) BINUTILS_VERSION="$2"; VERSION_PROVIDED=1; shift; shift;;
        -p | --prefix ) PREFIX="$2"; shift; shift;;
        -t | --target ) TARGET="$2"; shift; shift;;
        -- ) shift; break;;
        * ) break;;
    esac
done

if [ $VERSION_PROVIDED -eq 0 ]; then
    echo "No binutils version provided. Defaulting to binutils-${BINUTILS_VERSION}..."
fi

BINUTILS_NAME=binutils-${BINUTILS_VERSION}

if [ ! -f ${BINUTILS_NAME}.tar.* ]
then
	echo "No binutils tarball located. Downloading now..."
	wget https://ftp.gnu.org/gnu/binutils/${BINUTILS_NAME}.tar.bz2
	if [ $? != 0 ]; then
	    echo "Error downloading ${BINUTILS_NAME}. Exiting..."
	    exit -1
	fi
fi

if [ ! -d ${BINUTILS_NAME} ]
then
	echo "Unpacking ${BINUTILS_NAME}..." 
	tar xf ${BINUTILS_NAME}.tar.*
	if [ $? != 0 ]; then
	    echo "Error unpacking ${BINUTILS_NAME}. Exiting..."
	    exit -1
	fi
fi

if [ -z "${MAKE}" ]; then
    echo "MAKE not defined. Setting to gmake..."
    MAKE=gmake
fi

rm -rf build-binutils
mkdir build-binutils
cd build-binutils

../${BINUTILS_NAME}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls
${MAKE} -j7 
${MAKE} install

