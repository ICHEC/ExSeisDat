#!/bin/bash
source /etc/profile.d/modules.sh #So we can use the module command

set -u

DIR_NAME=$TEST_DIR/$(date +%s)$$
mkdir $DIR_NAME
cp -r $PIOL_DIR/src $DIR_NAME/

red='\e[31m'
DIR=$(pwd)
gfile=$PIOL_DIR/scripts/temp/gnuwarnings
ifile=$PIOL_DIR/scripts/temp/intelwarnings

cd $DIR_NAME

module purge
source $PIOL_DIR/mod_intel
make clean
make -j 24  -s 2> $ifile

module purge
make clean
source $PIOL_DIR/mod_gnu
make -j 24 -s 2> $gfile

RET=0
if [ -s $ifile ]; then
    echo $red Intel warnings not removed
    RET=1
    cat $ifile
fi

if [ -s $gfile ]; then
    RET=$RET+2
    echo $red GNU warnings not removed
    cat $gfile
fi
module purge

echo WarningsTest SRC $RET 0 0 > CHECK

