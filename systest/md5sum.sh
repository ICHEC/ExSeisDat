for file in /ichec/work/exseisdat/test/cathal/*
do
    cd $file
    pwd
    if [ -d util ]; then
        md5sum dat/* | cut -d ' ' -f 1  > newChecksum
        if [ ! -f CMP_CHECKSUM ]; then
            RET=4
        else
            cmp newChecksum CMP_CHECKSUM
            RET=$?
        fi
        echo " " $RET >> CHECK
    fi
    cd ..
done

