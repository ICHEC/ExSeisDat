#!/bin/bash
if [ $PIOL_SYSTEM == "Tullow" ]; then
#    TEST_DIR=/panfs/gpt2/MODELLING/DEVELOP/piotest/test/$USER
    TEST_DIR=/panfs/gpt2/SEAM_DATA/temp/$USER
else
    TEST_DIR=/ichec/work/exseisdat/test/$USER
fi

for file in $TEST_DIR/*
do
    cd $file
    pwd
    if [ -d util ]; then
        md5sum dat/* | cut -d ' ' -f 1  > newChecksum
        if [ ! -f CMP_CHECKSUM ]; then
            RET=4
        else
            cmp newChecksum CMP_CHECKSUM
            RET=$?
        fi
        echo " " $RET >> CHECK
    fi
    cd ..
done

