
#include "verlet.h"
#include "cloud.h"

#if defined(VERLET_K)
#define K VERLET_K
#else
#define K 16
#endif
void
verlet_step_accelerate (int Np, double dt, const double *restrict X[3], double *restrict U[3])
{
  int Nplim = Np - (Np % K);
  int i;

#if defined(GROUP_FORCE) && GROUP_FORCE
  #pragma omp parallel for schedule(static)
  // write updates for K particles at a time
  for (i = 0; i < Nplim; i += K) {
    double u[3][K] = {0.};
    double x[3][K];
    int j;

    // explicitly read the positions of the K particles into a temporary variable
    for (int l = 0; l < K; l++) {
      for (int d = 0; d < 3; d++) {
        x[d][l] = X[d][i + l];
      }
    }

    // read the other particles in groups of K as well
    for (j = 0; j < Nplim; j += K) {
      double dux[K][K];
      double duy[K][K];
      double duz[K][K];
      double y[3][K];

      // explicitly read the positions of the K particles into a temporary variable
      for (int m = 0; m < K; m++) {
        for (int d = 0; d < 3; d++) {
          y[d][m] = X[d][j + m];
        }
      }
      // COMPUTATION: we've set ourselves up for K^2 computations of force
      // without all data already read into the cache
      for (int l = 0; l < K; l++) {
        double ddux[K];
        double dduy[K];
        double dduz[K];
        force_vec (K, dt, x[0][l], x[1][l], x[2][l],
                   &(y[0][0]), &(y[1][0]), &(y[2][0]),
                   ddux, dduy, dduz);
        for (int m = 0; m < K; m++) {
          dux[l][m] = ddux[m];
          duy[l][m] = dduy[m];
          duz[l][m] = dduz[m];
        }
      }
      // zero out the interactions of a particle with itself
      if (i == j) {
        for (int l = 0; l < K; l++) {
          dux[l][l] = 0.;
          duy[l][l] = 0.;
          duz[l][l] = 0.;
        }
      }
      // sum the contributions of this block of particles
      for (int m = 0; m < K; m++) {
        for (int l = 0; l < K; l++) {
          u[0][l] += dux[l][m];
          u[1][l] += duy[l][m];
          u[2][l] += duz[l][m];
        }
      }
    }
    // remainder loop
    for (j = Nplim ; j < Np; j++) {
      double du[K][3];
      double y[3][K];

      for (int l = 0; l < K; l++) {
        force (dt, x[0][l], x[1][l], x[2][l], X[0][j], X[1][j], X[2][j], &(du[l][0]));
      }
      for (int l = 0; l < K; l++) {
        if (i + l == j) {
          for (int d = 0; d < 3; d++) {
            du[l][d] = 0.;
          }
        }
      }
      for (int l = 0; l < K; l++) {
        for (int d = 0; d < 3; d++) {
          u[d][l] += du[l][d];
        }
      }
    }
    for (int l = 0; l < K; l++) {
      for (int d = 0; d < 3; d++) {
        U[d][i + l] += u[d][l];
      }
    }
  }
  // remainder loop
  for (i = Nplim; i < Np; i++) {
    double u[3] = {0.};

    for (int j = 0; j < Np; j++) {
      if (j != i) {
        double du[3];

        force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

        for (int d = 0; d < 3; d++) {
          u[d] += du[d];
        }
      }
    }
    for (int d = 0; d < 3; d++) {
      U[d][i] += u[d];
    }
  }
#else
  #pragma omp parallel for schedule(static)
  for (i = 0; i < Np; i++) {
    double u[3] = {0.};

    for (int j = 0; j < Np; j++) {
      if (j != i) {
        double du[3];

        force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

        for (int d = 0; d < 3; d++) {
          u[d] += du[d];
        }
      }
    }
    for (int d = 0; d < 3; d++) {
      U[d][i] += u[d];
    }
  }
#endif
}

