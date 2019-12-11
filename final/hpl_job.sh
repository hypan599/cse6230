#PBS -N final-hpl
#PBS -j oe

# load module
module use ${CSE6230_DIR}/modulefiles
module load cse6230
HPL_DIR="$HOME/hpl"

PROFILE="1"
NP=1

# prepare dat file
rm -vrf $HPL_DIR/$PROFILE
rm -v $HOME/cse6230/final/result-"$PROFILE".txt
mkdir $HPL_DIR/$PROFILE
cp -v $HPL_DIR/xhpl $HPL_DIR/$PROFILE/xhpl
cp -v $HOME/cse6230/final/HPL-$PROFILE.dat $HPL_DIR/$PROFILE/HPL.dat

# run
cd $HPL_DIR/$PROFILE
mpirun -np $NP $HPL_DIR/$PROFILE/xhpl > $HPL_DIR/$PROFILE/HPL-"$PROFILE".out

grep WR $HPL_DIR/$PROFILE/HPL-"$PROFILE".out | tee $HOME/cse6230/final/result-"$PROFILE".txt
