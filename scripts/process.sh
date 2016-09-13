#ctestReadWrite largesegy.segy 0 24 48
#For each line, create a google-test function
TEST_DIR=/ichec/work/exseisdat/test
cat > tests.cc << EOL
#include "gtest/gtest.h"
#include "gmock/gmock.h"
EOL
j=0
for dir in $TEST_DIR/*
do
    export NAME

    if [ -f $dir/CHECK ]; then
        read -r NAME FILE SUCCESS PPN PPNN < <(head -n 1 $dir/CHECK)
    else
        NAME=FAIL$j
        FILE=FAIL
        SUCCESS=3
        PPN=0
        PPNN=0
        j=${j+1}
    fi

    if [ -f $dir/TIME ]; then
        read -r TIME < <(tail -n 1 $dir/TIME | cut -d ' ' -f 1)
    else
        TIME=NAN
    fi

    cat >> tests.cc << EOL
TEST($NAME, $(basename $FILE .segy)${PPN}${PPNN})
{
    ASSERT_EQ(0, $SUCCESS);
    std::cout << "runtime " << "$TIME" << std::endl;
}
EOL
done

#compile
make -j 2 > /dev/null

#run
./test

#remove & cleanup
make clean > /dev/null
if [ -n $TEST_DIR ]; then
echo rm -r -I "$TEST_DIR/*"
rm -r -I $TEST_DIR/*
echo rm -r -I "TEST*"
rm -r -I TEST*
fi

rm -f tests.cc
rm -f tasks.sh
rm -f str.awk

