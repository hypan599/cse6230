#include "dmv.h"
#include "dmv_impl.h"

int DenseMatVec_2dPartition(Args args, int mStart, int mEnd, int nStart, int nEnd, const double *matrixEntries, int rStart, int rEnd, const double *vecRightLocal, int lStart, int lEnd, double *vecLeftLocal)
{
  
  MPI_Comm comm = args->comm;
  int      size, rank, err;

  err = MPI_Comm_size(comm, &size); MPI_CHK(err);
  err = MPI_Comm_rank(comm, &rank); MPI_CHK(err);
  /* TODO: implement a matrix-vector multiplication on a 2d matrix partition */
  /* HINT:
   * 1. Use DMVCommGetRankCoordinates2D() to get the coordinates of the current rank in a 2d grid of MPI processes.
   * 2. Use the coordinates as colorings to split the communicator into row and column communicators.
   * 3. Use MPI_Allgatherv() on the column communicator so that every process has the correct entries to multiply against its local portion of the matrix ([nStart, nEnd]).
   *      Look at DenseMatVec_RowPartition_Allgatherv() in dmv_matvec_row.c for an example of using MPI_Algatherv() in this way, but adapt it to the column communicator.
   *      You will need to allocate temporary space for the result of the allgatherv, and free it when you are done with it.
   * 4. Compute the local portion of the matrix vector product.
   * 5. Use MPI_Reduce_scatter() on the row communicator to sum all of the row contributions to vecLeftLocal.
   *      Look at DenseMatVec_ColPartition() in dmv_matvec_col.c for an example of use MPI_Reduce_scatter() in this wary, but adapt it to the row communicator.
   */
  printf("%d-th node:\tmStart: %d\t, mEnd: %d\t, nStart: %d\t, nEnd: %d\t, lStart: %d\t, lEnd: %d\t, rStart: %d\t, rEnd: %d\n",rank, mStart, mEnd, nStart, nEnd, lStart, lEnd, rStart, rEnd);
  // step1
  int numRows, row, numCols, col;
  numRows = numCols = row = col = -1;
  err = DMVCommGetRankCoordinates2D(comm, &numRows, &row, &numCols, &col); MPI_CHK(err);

  // step2
  MPI_Comm colComm, rowComm;
  err = MPI_Comm_split(comm, col, rank, &colComm); MPI_CHK(err);
  err = MPI_Comm_split(comm, row, rank, &rowComm); MPI_CHK(err);

  // step3
  int colCommSize = 0;
  int rowCommSize = 0;
  err = MPI_Comm_size(colComm, &colCommSize); MPI_CHK(err);
  err = MPI_Comm_size(rowComm, &rowCommSize); MPI_CHK(err);

  int nRightLocal = rEnd - rStart;
  double* temp_vec_right;
  temp_vec_right = (double *)malloc((mEnd - mStart) * sizeof(double));
  if (!temp_vec_right) MPI_CHK(1);
  double* vecLeft;
  vecLeft = (double *) malloc((mEnd - mStart) * sizeof(double));
  if (!vecLeft) MPI_CHK(1);
  int* nLocals;
  nLocals = (int *) malloc(colCommSize * sizeof(int));
  if (!nLocals) MPI_CHK(1);
  int* nOffsets;
  nOffsets = (int *) malloc((colCommSize + 1) * sizeof(int));
  if (!nOffsets) MPI_CHK(1);

  // Gather the counts for every process
  err = MPI_Allgather(&nRightLocal, 1, MPI_INT, nLocals, 1, MPI_INT, colComm); MPI_CHK(err);
  // Turn the counts into the offsets
  nOffsets[0] = 0;
  for (int q = 0; q < colCommSize; q++) {
    nOffsets[q + 1] = nOffsets[q] + nLocals[q];
  }
  err = MPI_Allgatherv(vecRightLocal, nRightLocal, MPI_DOUBLE, temp_vec_right, nLocals, nOffsets, MPI_DOUBLE, colComm);
  MPI_CHK(err);

  // step4
  int num_cols = mEnd - mStart;
  for (int r = 0; r < nEnd - nStart; r++){
    double val = 0;
    for (int c = 0; c < mEnd - mStart; c++){
      val += matrixEntries[r * num_cols + c] * temp_vec_right[c];
    }
    vecLeft[r] = val;
  }

  // step5
  int* mLocals;
  mLocals = (int *) malloc(rowCommSize * sizeof(int));
  if (!mLocals) MPI_CHK(1);
  err = MPI_Allgather(&num_cols, 1, MPI_INT, mLocals, 1, MPI_INT, rowComm); MPI_CHK(err);
  err = MPI_Reduce_scatter(vecLeft, vecLeftLocal, mLocals, MPI_DOUBLE, MPI_SUM, rowComm); MPI_CHK(err);

  // final clean;
  err = MPI_Comm_free(&rowComm);MPI_CHK(err);
  err = MPI_Comm_free(&colComm);MPI_CHK(err);
  free(nOffsets);
  free(nLocals);

  return 0;
}
