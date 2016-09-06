
#    copy src files and the utility
#C
cd $1

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
time make -j 24
cd ../api
time make
cd ../util
time make -j 24 $NAME
echo build finished.

