#PBS -N final-hpl
#PBS -j oe

# load module
module use ${CSE6230_DIR}/modulefiles
module load cse6230
HPL_DIR="$HOME/hpl"

if [ $# == "0" ]; then
    PROFILE="16"
else
    PROFILE=$1
fi

# prepare dat file
rm -v $HPL_DIR/HPL.dat $HPL_DIR/"HPL-$PROFILE.out"
cp -v ./HPL-$PROFILE.dat $HPL_DIR/HPL.dat

# run
mpirun -np $PROFILE "$HPL_DIR/xhpl" > "HPL-$PROFILE.out"
