#!/bin/bash
#TODO: Safety checks
read -r NODE_COUNT NODE_PPN MPI NAME FILENAME STRIPE_COUNT MODULE < <(echo "$@")

#global variables
source /etc/profile.d/modules.sh #So we can use the module command

source ../mod_$MODULE

PPN_COMMAND="-ppn"

if [ $MPI != "intel" ]; then
module load $MPI
fi

export MPI_BASE=$(echo $MPI | cut -d \/ -f 1)

if [ $MPI_BASE == "openmpi" ]; then
    PPN_COMMAND="-npernode"

    if [ $MODULE == intel ]; then
        export OMPI_CXX=icpc
        export OMPI_CC=icc
    fi
fi

if [ $MPI_BASE == "mpich2" ]; then
    if [ $MODULE == "intel" -o $MODULE == "tullow" ]; then
        export MPICH_CXX=icpc
        export MPICH_CC=icc
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

if [ $PIOL_SYSTEM != "Tullow" ]; then
    lfs setstripe -c $STRIPE_COUNT dat
    else
#TODO: I am not aware of any panasas ability to set stripe counts
#      on a per file basis.
    echo Can not set stripes to $STRIPE_COUNT - Panasas
fi

bash make.sh $NAME

#    run the test
mv util/$NAME .

echo $(which time) -f "%e %I %O %M %W" mpirun $PPN_COMMAND $NODE_PPN $NAME $ARGUMENTS > ARGUMENTS

if [ -f $NAME ]; then
    if [ $PIOL_SYSTEM == "Tullow" ]; then
        head -n $NODES hosts.txt > hostsfinal.txt
        $(which time) -f "%e %I %O %M %W" mpirun -f hostsfinal.txt $PPN_COMMAND $NODE_PPN $NAME $ARGUMENTS 2> TIME
    else
        $(which time) -f "%e %I %O %M %W" mpirun $PPN_COMMAND $NODE_PPN $NAME $ARGUMENTS 2> TIME
    fi
else
    echo FILE DID NOT COMPILE
fi

#    checksum
#    record pass/fail
md5sum dat/$OUTPUT | cut -d ' ' -f 1  > newChecksum
if [ ! -f $PIOL_DIR/checksum/checksum_$(basename $FILENAME)_$NAME ]; then
RET=4
else
cmp newChecksum $PIOL_DIR/checksum/checksum_$(basename $FILENAME)_$NAME
RET=$?
fi
if [ -z $PBS_NODEFILE ]; then
    echo $NAME$NODE_COUNT$NODE_PPN$MPI_BASE$STRIPE_COUNT$MODULE $(basename $FILENAME .segy) $RET $NODE_PPN $(expr $NODES \* $NODE_PPN) > CHECK
else
    echo $NAME$NODE_COUNT$NODE_PPN$MPI_BASE$STRIPE_COUNT$MODULE $(basename $FILENAME .segy) $RET $NODE_PPN $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
fi
