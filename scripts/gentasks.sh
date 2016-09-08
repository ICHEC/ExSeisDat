rm -f tasks.sh

while read f; do
    str='{ print "bash farm.sh " $1 " '
    str+=$f
    str+=" \" }"
    echo $str > str.awk
    ls test_f_*.sh | cut -f 3 -d _ | cut -f 1 -d . | awk -f str.awk >> tasks.sh
done<filelist

ls test_n_*.sh | cut -f 3 -d _ | cut -f 1 -d . | awk '{ print "bash farm.sh " $1 }' >> tasks.sh
