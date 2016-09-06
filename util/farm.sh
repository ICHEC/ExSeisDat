#!/bin/bash
#TODO: Safety checks

#########SETTINGS#########
source genenv.sh
source ctestReadWrite.sh
export STRIPE_COUNT=10
DIR_NAME=$TEST_DIR/$NAME$(date +%s)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_NAME/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR_NAME/lib
##########################

#    Module loading
source $PIOL_DIR/mod_$MODULE

bash dir.sh $DIR_NAME
bash make.sh $DIR_NAME

#    run the test
cd $DIR_NAME
mv util/$NAME .
time mpirun -ppn 20 $NAME $ARGUMENTS

#    checksum
#    record pass/fail
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum
cmp newChecksum $PIOL_DIR/checksum/checksum_largesegy.segy_ctestReadWrite
echo $? > CHECK

