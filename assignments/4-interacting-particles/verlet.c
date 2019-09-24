#include <math.h>
#include <omp.h>
#include "cloud.h"
#include "verlet.h"

void
verlet_step (int Np, int Nt, double dt, double k, double d,
             cse6230rand_t *rand,
             double *restrict X[3],
             double *restrict U[3], double *accl_time, double *strm_time)
{
  int t;
  double dt_accel = k * dt;
  double dt_half  = 0.5 * dt;
  double dt_fullnoise = sqrt(2. * d * dt);
  double dt_halfnoise = sqrt(d * dt);
  double accl_tic, accl_toc;
  double strm_tic, strm_toc;

  if (Nt > 0) {
    strm_tic = omp_get_wtime();
    verlet_step_stream_and_noise (Np, dt_half, dt_halfnoise, rand, X, (const double *restrict *) U);
    strm_toc = omp_get_wtime();
    *strm_time += strm_toc - strm_tic;
  }
  for (t = 0; t < Nt - 1; t++) {
    if (dt_accel) {
      accl_tic = omp_get_wtime();
      verlet_step_accelerate (Np, dt_accel, (const double *restrict *) X, U);
      accl_toc = omp_get_wtime();
      *accl_time += accl_toc - accl_tic;
    }
    strm_tic = omp_get_wtime();
    verlet_step_stream_and_noise (Np, dt, dt_fullnoise, rand, X, (const double *restrict *) U);
    strm_toc = omp_get_wtime();
    *strm_time += strm_toc - strm_tic;
  }
  for (; t < Nt; t++) {
    if (dt_accel) {
      accl_tic = omp_get_wtime();
      verlet_step_accelerate (Np, dt_accel, (const double *restrict *) X, U);
      accl_toc = omp_get_wtime();
      *accl_time += accl_toc - accl_tic;
    }
    strm_tic = omp_get_wtime();
    verlet_step_stream_and_noise (Np, dt_half, dt_halfnoise, rand, X, (const double *restrict *) U);
    strm_toc = omp_get_wtime();
    *strm_time += strm_toc - strm_tic; }
}

