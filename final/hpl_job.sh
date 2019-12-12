#PBS -N final-hpl
#PBS -j oe

# load module
module use ${CSE6230_DIR}/modulefiles
module load cse6230
HPL_DIR="$HOME/hpl"

PROFILE="25"
NP=25

# prepare dat file, copy and make new dir for each run so that they don't corrupt
# available config files are HPL_*.dat. Here the number don't have much meaning, they're just identifiers
rm -vrf $HPL_DIR/$PROFILE
rm -v $HOME/cse6230/final/result-"$PROFILE".txt
mkdir $HPL_DIR/$PROFILE
cp -v $HPL_DIR/xhpl $HPL_DIR/$PROFILE/xhpl
cp -v $HOME/cse6230/final/HPL-$PROFILE.dat $HPL_DIR/$PROFILE/HPL.dat

# run
cd $HPL_DIR/$PROFILE
mpirun -np $NP $HPL_DIR/$PROFILE/xhpl > $HPL_DIR/$PROFILE/HPL-"$PROFILE".out

# gather result
grep WR $HPL_DIR/$PROFILE/HPL-"$PROFILE".out > $HOME/cse6230/final/result-"$PROFILE".txt
