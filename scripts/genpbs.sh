#for i in `seq 1 5`;
for i in `seq 1`;
do
    echo $NODE_LIST
    for j in $NODE_LIST;
    do
       qsub -v PROC_COUNT=$j -l nodes=$i:ppn=24,walltime=0:00:10:00 utiltest.pbs
    done
done
