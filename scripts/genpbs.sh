for i in `seq 1 5`;
do
    qsub -l nodes=$i:ppn=24,walltime=0:00:05:00 utiltest.pbs
done
