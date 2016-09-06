rm tasks.sh

INPUT=''

for f in $1/*; do
    str='{ print "bash farm.sh " $1 " '
    str+=$f
    str+=" \" }"
    echo $str > str.awk
    ls test_*.sh | cut -f 2 -d _ | cut -f 1 -d . | awk -f str.awk >> tasks.sh
done

rm str.awk

