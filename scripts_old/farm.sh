#!/bin/bash
#TODO: Safety checks

#########SETTINGS#########
source genenv.sh
if [[ -z $2 ]]; then
    if [ ! -e test_n_$1.sh ]; then
        echo script test_n_$1.sh does not exist
        exit -1
    fi
    echo No second argument $2
    source test_n_$1.sh
    FILENAME=EMPTY
else
    if [ ! -e test_f_$1.sh ]; then
        echo script test_f_$1.sh does not exist
        exit -1
    fi
    source test_f_$1.sh $2
    FILENAME=$(basename $2)
fi

DIR_NAME=$TEST_DIR/$(date +%s)$$
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_NAME/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR_NAME/lib
##########################

echo $DIR_NAME

bash dir.sh $DIR_NAME
bash make.sh $DIR_NAME

#    run the test
cd $DIR_NAME
mv util/$NAME .
echo here $1 $FILENAME
echo $(which time) -f "%e %I %O %M %W" mpirun -ppn $PROC_COUNT $NAME $ARGUMENTS 2> TIME
$(which time) -f "%e %I %O %M %W" mpirun -ppn $PROC_COUNT $NAME $ARGUMENTS 2> TIME

#    checksum
#    record pass/fail
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum
cmp newChecksum $PIOL_DIR/checksum/checksum_${FILENAME}_$1
echo $1 $FILENAME $? $PROC_COUNT $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
