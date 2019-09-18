
#include "verlet.h"
#include "cloud.h"

void
verlet_step_accelerate (int Np, double dt, const double *restrict X[3], double *restrict U[3])
{
    int Nthreads = 8;
    for (int i = 0; i < Np; i++) {
        double u[3] = {0.};
        #pragma omp parallel for
        for (int l = 0; l < Np; l+=N) {
            double outer_du[3] = {0.};
            for (int k = 0; k < N, l + k < Np; k += 1) {  // non parallelable
                int j = l + k;
                if (j != i) {
                    double du[3];
                    force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);
                    outer_du[0] += du[0];
                    outer_du[1] += du[1];
                    outer_du[2] += du[2];
                }
            }

            u[d] += outer_du[d];
            u[d] += outer_du[d];
            u[d] += outer_du[d];
        }
        // #pragma omp parallel for
        // for (int d = 0; d < 3; d++) {
        //     U[d][i] += u[d];
        // }
        U[0][i] += u[0];
        U[1][i] += u[1];
        U[2][i] += u[2];
    }
}

// void
// verlet_step_accelerate (int Np, double dt, const double *restrict X[3], double *restrict U[3])
// {
//   for (int i = 0; i < Np; i++) {
//     double u[3] = {0.};

//     for (int j = 0; j < Np; j++) {
//       if (j != i) {
//         double du[3];

//         force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

//         for (int d = 0; d < 3; d++) {
//           u[d] += du[d];
//         }
//       }
//     }
//     for (int d = 0; d < 3; d++) {
//       U[d][i] += u[d];
//     }
//   }
// }