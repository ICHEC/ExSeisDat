#ctestReadWrite largesegy.segy 0 24 48
#For each line, create a google-test function
TEST_DIR=/ichec/work/exseisdat/test
cat > tests.cc << EOL
#include "gtest/gtest.h"
#include "gmock/gmock.h"
EOL

#TEMP=$(mktemp)
#cat $TEST_DIR/*/CHECK > $TEMP

for dir in $TEST_DIR/*
do
    read -r NAME FILE SUCCESS PPN PPNN < <(head -n 1 $dir/CHECK)
    cat >> tests.cc << EOL
TEST($NAME, $(basename $FILE .segy)${PPN}${PPNN})
{
    ASSERT_EQ(0, $SUCCESS);
}
EOL

    read -r TIME < <(cut -d ' ' -f 1 $dir/TIME)
done

#compile
make -j 2 > /dev/null

#run
./test

#remove & cleanup
make clean > /dev/null
rm -f tests.cc
if [ -n $TEST_DIR ]; then
echo To delete test directories, type rm -r "$TEST_DIR/*"
fi

