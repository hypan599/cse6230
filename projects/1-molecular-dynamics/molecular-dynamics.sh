
module use $CSE6230_DIR/modulefiles
module load cse6230/core

# module use $CSE6230_DIR/modulefiles
# module load cse6230/gcc-omp-gpu

pygmentize steric.h

sed -n '54,85 p' accelerate.c | pygmentize -l c

# export OMP_NUM_THREADS=...
# export OMP_PROC_BIND=...
# export OMP_SCHEDULE=...
# etc.

for N_p in 1 2 4 8 16 32; do
  this_L=`echo "$N_p 0.333 20." | awk '{ print ($3 * $1^$2); }'`
  this_T=`echo "$N_p 25600" | awk '{ print ($2 / ($1 * $1)); }'`
  make runcloud NP=$(( 256*$N_p )) L=$this_L NT=$this_T PERF="perf stat"
done

make checkcloud NP=512 L=25.198421 NT=51000 CHUNK=1000
