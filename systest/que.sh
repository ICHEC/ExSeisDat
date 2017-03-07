rm -f temp/*

for file in testmatrix_*param; do
  num=$(echo $file | tr -dc '0-9')
  awk "{ print \$0 > \"temp/util_${num}_\"\$1\".sh\" }" $file
done

while read -r NODES ; do
{
    if [ $PIOL_SYSTEM == "Tullow" ]; then
        NODES=$NODES bash runtest.pbs
    else
        qsub -v NODES=$NODES -l nodes=$NODES:ppn=24,walltime=0:02:00:00 runtest.pbs
    fi
}
done < <(ls temp/* | cut -d "_" -f 3 | cut -d "." -f 1 | sort -u)

shopt -s nocasematch
if [[ -z "$1" ]]; then
if [[ $1 == "n" ]]; then
    exit
fi
fi

shopt -u nocasematch

if [[ $PIOL_SYSTEM == "Tullow" ]]; then
    echo Skip makefile check
    exit
fi

source /etc/profile.d/modules.sh
source ../mod_gnu
export PIOL_DIR=$PWD/..
export TEST_DIR=/ichec/work/exseisdat/test/$USER
bash warncheck.sh
