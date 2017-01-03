for file in /ichec/work/exseisdat/test/*
do
 cd $file
 pwd
 md5sum dat/* | cut -d ' ' -f 1  > newChecksum
 if [ ! -f CMP_CHECKSUM ]; then
    RET=4
 else
 cmp newChecksum CMP_CHECKSUM
    RET=$?
 fi
 echo -n $RET # >> CHECK
 cd ..
done

