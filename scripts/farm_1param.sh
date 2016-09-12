#!/bin/bash
#TODO: Safety checks
read -r NODE_COUNT NODE_PPN MPI NAME FILENAME STRIPE_COUNT MODULE < <(echo "$@")

#global variables
source /etc/profile.d/modules.sh #So we can use the module command

source ../mod_$MODULE

if [ $MPI == mvapich ] ; then
    module load mvapich2/intel/2.1a
elif [ $MPI == openmpi ]; then
    module load openmpi/intel/1.8.3
fi

#########SETTINGS#########
if [ ! -e test_f_$NAME.sh ]; then
    echo script test_f_$NAME.sh does not exist
    exit -1
fi
source test_f_$NAME.sh $FILENAME

export TEST_DIR=/ichec/work/exseisdat/test
export PIOL_DIR=$PWD/..
export C_INCLUDE_PATH=$PIOL_DIR/api
export CPLUS_INCLUDE_PATH=$PIOL_DIR/include
DIR_NAME=$TEST_DIR/$(date +%s)$$
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_NAME/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR_NAME/lib
#########END SETTINGS#########

#    Make test directory
mkdir $DIR_NAME
echo $DIR_NAME
#    Making sub-directories
cp make.sh $DIR_NAME/
cd $DIR_NAME
mkdir src dat api util lib
mkdir src/obj api/obj util/obj
lfs setstripe -c $STRIPE_COUNT dat

bash make.sh $NAME

#    run the test
mv util/$NAME .
echo $(which time) -f "%e %I %O %M %W" mpirun -ppn $NODE_PPN $NAME $ARGUMENTS 2> TIME
$(which time) -f "%e %I %O %M %W" mpirun -ppn $NODE_PPN $NAME $ARGUMENTS 2> TIME

#    checksum
#    record pass/fail
echo MD5
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum
cmp newChecksum $PIOL_DIR/checksum/checksum_$(basename $FILENAME)_$NAME
RET=$?
echo CHECK
echo $NAME$NODE_COUNT$NODE_PPN$MPI$STRIPE_COUNT$MODULE $(basename $FILENAME .segy) $RET $NODE_PPN $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
echo DONE
