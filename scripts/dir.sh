#    Make test directory
#    Making sub-directories
#    Move input files / links
mkdir $1
cd $1

mkdir src dat api util lib
mkdir src/obj api/obj util/obj
lfs setstripe -c $STRIPE_COUNT dat

for i in $DIR_FILES; do
    ln -s $FILES_DIR/$i dat/
done

