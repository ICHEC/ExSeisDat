for i in `seq 1 5`;
do
    echo $NODE_LIST
    for j in $NODE_LIST;
    do
#TODO: Pass a list of process counts instead?
       qsub -v PROC_COUNT=$j -l nodes=$i:ppn=24,walltime=0:02:00:00 utiltest$1.pbs
    done
done
