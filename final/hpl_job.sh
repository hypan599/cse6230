#PBS -N final-hpl
#PBS -j oe

# load module
module use ${CSE6230_DIR}/modulefiles
module load cse6230
HPL_DIR="$HOME/hpl"

PROFILE="251"

# prepare dat file
rm -v $HPL_DIR/HPL.dat $HPL_DIR/HPL-"$PROFILE".out $HOME/cse6230/final/result-"$PROFILE".txt
cp -v $HOME/cse6230/final/HPL-$PROFILE.dat $HPL_DIR/HPL.dat

# run
mpirun -np $PROFILE $HPL_DIR/xhpl > HPL-"$PROFILE".out

grep WR $HPL_DIR/HPL-"$PROFILE".out | tee $HOME/cse6230/final/result-"$PROFILE".txt
