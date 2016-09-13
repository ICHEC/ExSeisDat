#!/bin/bash
if [ ! -e test_n_$4.sh ]; then
    echo script test_n_$4.sh does not exist
    exit -1
fi
source test_n_$4.sh $OUTPUT
source farm.sh "$@"
