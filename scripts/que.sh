rm -if temp/*
awk "{ print \$0 > \"temp/util_1_\"\$1\".sh\" }" testmatrix
while read -r NODES ; do
    qsub -v NODES=$NODES -l nodes=$NODES:ppn=24,walltime=0:02:00:00 runtest.pbs
done < <(ls temp/* | cut -d "_" -f 3 | cut -d "." -f 1 )
