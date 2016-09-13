
#    copy src files and the utility
#C

if [ -z $PIOL_DIR ]
then
    echo PIOL_DIR is not defined
fi

cp $PIOL_DIR/util/*.c util/
cp $PIOL_DIR/util/*.h util/
cp $PIOL_DIR/util/*.cc util/
cp $PIOL_DIR/util/*.hh util/
cp $PIOL_DIR/util/makefile util/

cp $PIOL_DIR/api/cfileapi.cc api/
cp $PIOL_DIR/api/cfileapi.h api/
cp $PIOL_DIR/api/makefile api/

cp $PIOL_DIR/src/*.cc src/
cp $PIOL_DIR/src/makefile src/

cp $PIOL_DIR/makefile .
cp $PIOL_DIR/compiler.cfg . 

#    compile
cd src
make -j 24 > /dev/null
cd ../api
make > /dev/null
cd ../util
make -j 24 $1 > /dev/null

