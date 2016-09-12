#Test description
export NAME=makerep
export INPUT=$1
export OUTPUT=test1.segy
export ARGUMENTS="-i $INPUT -o dat/$OUTPUT -v standard -r 1 " #TODO: Construct argument
export MODULE=intel                             #gnu, intel, tullow
export DIR_FILES=$1
