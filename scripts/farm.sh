#!/bin/bash
#TODO: Safety checks

#########SETTINGS#########
source genenv.sh
source test_$1.sh $2
export STRIPE_COUNT=10
DIR_NAME=$TEST_DIR/$(date +%s)$$
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
$(which time) -f "%e %I %O %M %W" mpirun -ppn $PROC_COUNT $NAME $ARGUMENTS 2> TIME

#    checksum
#    record pass/fail
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum

#Get the filename
cmp newChecksum $PIOL_DIR/checksum/checksum_$(basename $2)_$1
echo $1 $(basename $2) $? $PROC_COUNT $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
