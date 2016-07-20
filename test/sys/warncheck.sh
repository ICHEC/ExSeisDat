#!/bin/bash
#Begining of a system check. Need actual time allocation to work on it
source /etc/profile.d/modules.sh #So we can use the module command

red='\e[31m'
DIR=$(pwd)
TLD=$DIR/../../
gfile=$DIR/../tmp/gnuwarnings
ifile=$DIR/../tmp/intelwarnings
cd $DIR/../../src

module purge
source $TLD/mod_intel
make -s 2> $ifile

module purge
source $TLD/mod_gnu
make -s 2> $gfile

if [ -s $ifile ]; then
    echo $red Intel warnings not removed
fi

if [ -s $gfile ]; then
    echo $red GNU warnings not removed
fi

