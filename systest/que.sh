rm -if temp/*

if [ -f  testmatrix_1param ]; then
awk "{ print \$0 > \"temp/util_1_\"\$1\".sh\" }" testmatrix_1param
fi
if [ -f  testmatrix_0param ]; then
awk "{ print \$0 > \"temp/util_0_\"\$1\".sh\" }" testmatrix_0param
fi

while read -r NODES ; do
    qsub -v NODES=$NODES -l nodes=$NODES:ppn=24,walltime=0:00:30:00 runtest.pbs
done < <(ls temp/* | cut -d "_" -f 3 | cut -d "." -f 1 | sort -u)

export PIOL_DIR=$PWD/..
export TEST_DIR=/ichec/work/exseisdat/test
bash warncheck.sh
