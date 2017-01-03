#!/bin/bash
source /etc/profile.d/modules.sh #So we can use the module command

DIR_NAME=$TEST_DIR/$(date +%s)$$

mkdir $DIR_NAME

cp -r $PIOL_DIR/src $DIR_NAME/
cp -r $PIOL_DIR/compiler.cfg $DIR_NAME/
ln -s $PIOL_DIR/include $DIR_NAME/
mkdir $DIR_NAME/lib
cd $PIOL_DIR/src

red='\e[31m'
DIR=$(pwd)
gfile=$PIOL_DIR/systest/temp/gnuwarnings
ifile=$PIOL_DIR/systest/temp/intelwarnings

cd $DIR_NAME/src

module purge
source $PIOL_DIR/mod_intel
make -s clean
make -j 24  -s 2> $ifile

module purge
source $PIOL_DIR/mod_gnu
make -s clean
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

echo WarningsTest SRC 0 0 $RET > ../CHECK
echo 0 0 0 0 0 0 0 0 > ../TIME

