#!/bin/bash
#TODO: Safety checks
#!/bin/bash
if [ ! -e test_f_$4.sh ]; then
    echo script test_f_$4.sh does not exist
    exit -1
fi
source test_f_$4.sh $5 $OUTPUT
source farm.sh "$@"
