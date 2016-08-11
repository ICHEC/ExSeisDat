module load apps taskfarm/2.7
export TASKFARM_PPN=24
export TASKFARM_SMT=1

#cd .. 
#make clean > /dev/null
#time make -j 12 > /dev/null
#cd -

taskfarm tests.lst

cat test? >> test_$1
rm test?

