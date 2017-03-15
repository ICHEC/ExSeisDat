#!/bin/bash
if [ $PIOL_SYSTEM == "Tullow" ]; then
    TEST_DIR=/panfs/gpt2/SEAM_DATA/temp/$USER
else
    TEST_DIR=/ichec/work/exseisdat/test/$USER
fi

for file in $TEST_DIR/*
do
    cd $file
    pwd
    if [ -d util ]; then
        read -r str < <(head -n 1 CHECK)
        set -- $str
        if [[ $# == 4 ]]; then
            md5sum dat/* | cut -d ' ' -f 1  > newChecksum
            if [ ! -f CMP_CHECKSUM ]; then
                RET=4
            else
                cmp newChecksum CMP_CHECKSUM
                RET=$?
            fi
            echo " " $RET >> CHECK
        fi
    else
        echo Skip $file
    fi
    cd ..
done

