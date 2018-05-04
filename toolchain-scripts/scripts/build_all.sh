#!/usr/bin/env bash

source ./getopt.bash

OPTS=`getopt -o b:g:p:t: --long binutils:,gcc:,prefix:,target: -n 'parse-options' -- "$@"`

if [ $? != 0 ]; then
    echo "Failed parsing options." >&2
    exit 1
fi

echo "build_all.sh: $OPTS"
eval set -- "$OPTS"

PREFIX=$HOME/.cross
TARGET=i586-elf
PATH="$PREFIX/bin:$PATH"

while true; do
    case "$1" in
        -b | --binutils ) BINUTILS_VERSION="$2"; shift; shift;;
        -g | --gcc ) GCC_VERSION="$2"; shift; shift;;
        -p | --prefix ) PREFIX="$2"; shift; shift;;
        -t | --target ) TARGET="$2"; shift; shift;;
        -- ) shift; break;;
        * ) break;;
    esac
done

#export CC=gcc-7
#export CXX=g++-7
#export MAKE=make

if [ ! -z "$BINUTILS_VERSION" ]; then
    BINUTILS_OPTS="--version=$BINUTILS_VERSION"
fi

if [ ! -z "$GCC_VERSION" ]; then
    GCC_OPTS="--version=$GCC_VERSION"
fi

BINUTILS_OPTS="$BINUTILS_OPTS --target=$TARGET --prefix=$PREFIX"
GCC_OPTS="$GCC_OPTS --target=$TARGET --prefix=$PREFIX"

./build_binutils.sh $BINUTILS_OPTS
./build_gcc.sh $GCC_OPTS

