#!/bin/bash
#TODO: More safety checks

read -r NODE_COUNT NODE_PPN MPI NAME STRIPE_COUNT MODULE FILENAME1 FILENAME2 < <(echo "$@")

#global variables
source /etc/profile.d/modules.sh #So we can use the module command
source ../mod_$MODULE

if [ $MPI != "intel" ]; then
    module load $MPI
fi

export MPI_BASE=$(echo $MPI | cut -d \/ -f 1)

if [ $MPI_BASE == "openmpi" ]; then
    PPN_COMMAND="--map-by ppr:$NODE_PPN:node"
else
    PPN_COMMAND="-ppn $NODE_PPN"
fi

if [ $MODULE == "intel" -o $MODULE == "tullow" ]; then
    export MPICH_CXX=icpc
    export MPICH_CC=icc
fi

#########SETTINGS#########
DIR_NAME=$TEST_DIR/$(date +%s)$$
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_NAME/lib
export LIBRARY_PATH=$LIBRARY_PATH:$DIR_NAME/lib
export C_INCLUDE_PATH=$C_INCLUDE_PATH:$PWD/api:$PIOL_DIR/include
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$PWD/api:$PIOL_DIR/include
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

set -f
echo $(which time) -f "%e %I %O %M %W" mpirun $PPN_COMMAND $NAME $ARGUMENTS > ARGUMENTS
set +f

if [ -f $NAME ]; then
    if [ $PIOL_SYSTEM == "Tullow" ]; then
      head -n $NODES hosts.txt > hostsfinal.txt
      set -f
      $(which time) -f "%e %I %O %M %W" mpirun -f hostsfinal.txt $PPN_COMMAND $NAME $ARGUMENTS 2> TIME > MSG
      set +f
    else
      set -f
      LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib $(which time) -f "%e %I %O %M %W" mpirun $PPN_COMMAND $NAME $ARGUMENTS 2> TIME > MSG
      set +f
    fi
else
    echo FILE DID NOT COMPILE
fi

if [[ -n FILENAME1 ]]; then 
    FILENAME1=$(basename $FILENAME1 .segy)
fi
if [[ -n FILENAME2 ]]; then 
    FILENAME2=$(basename $FILENAME2 .segy)
fi

cat $PIOL_DIR/checksum/checksum_$FILENAME1${FILENAME2}_$NAME > CMP_CHECKSUM
if [ -z $PBS_NODEFILE ]; then
    echo -n $NAME$NODE_COUNT$NODE_PPN$MPI_BASE$STRIPE_COUNT$MODULE $FILENAME1$FILENAME2 $NODE_PPN $(expr $NODES \* $NODE_PPN) > CHECK
else
    echo -n $NAME$NODE_COUNT$NODE_PPN$MPI_BASE$STRIPE_COUNT$MODULE $FILENAME1$FILENAME2 $NODE_PPN $(wc -l $PBS_NODEFILE | cut -d ' ' -f 1) > CHECK
fi
