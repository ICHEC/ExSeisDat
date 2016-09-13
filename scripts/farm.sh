#!/bin/bash
#TODO: Safety checks
read -r NODE_COUNT NODE_PPN MPI NAME FILENAME STRIPE_COUNT MODULE < <(echo "$@")

#global variables
source /etc/profile.d/modules.sh #So we can use the module command

source ../mod_$MODULE

PPN_COMMAND="-ppn"
if [ $MPI == mvapich ] ; then
    module load mvapich2/intel/2.1a
elif [ $MPI == openmpi ]; then
    PPN_COMMAND="-npernode"

    module load openmpi/gcc/1.8.4
    if [ $MODULE == intel ]; then
        export OMPI_CXX=icpc
        export OMPI_CC=icc
#        module load openmpi/intel/1.8.1
    fi
fi

#########SETTINGS#########
DIR_NAME=$TEST_DIR/$(date +%s)$$
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_NAME/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR_NAME/lib
#########END SETTINGS#########

#    Make test directory
mkdir $DIR_NAME
#    Making sub-directories
cp make.sh $DIR_NAME/
cd $DIR_NAME
mkdir src dat api util lib
mkdir src/obj api/obj util/obj
lfs setstripe -c $STRIPE_COUNT dat

module list
bash make.sh $NAME

#    run the test
mv util/$NAME .

if [ -f $NAME ]; then
    $(which time) -f "%e %I %O %M %W" mpirun $PPN_COMMAND $NODE_PPN $NAME $ARGUMENTS 2> TIME
else
    echo FILE DID NOT COMPILE
fi

#    checksum
#    record pass/fail
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum
cmp newChecksum $PIOL_DIR/checksum/checksum_$(basename $FILENAME)_$NAME
RET=$?
echo $NAME$NODE_COUNT$NODE_PPN$MPI$STRIPE_COUNT$MODULE $(basename $FILENAME .segy) $RET $NODE_PPN $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
module purge
