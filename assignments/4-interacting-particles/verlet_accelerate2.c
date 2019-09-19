
#include "verlet.h"
#include "cloud.h"

void
verlet_step_accelerate (int Np, double dt, const double *restrict X[3], double *restrict U[3])
{
  // try to divide up work evenly among workers
//   #pragma omp parallel for schedule(static)
//   for(int row = 0; row <= (Np-1) / 2; row ++ ){
//       for (int col = 0; col < Np; col ++) {
//           int i, j;
//           if (row == (Np-1) / 2) {
//               if (col <= row){
//                   continue;
//               } else {
//                   i = row;
//                   j = col;
//               }
//           }
//           if (col <= row){
//               i = Np - 2 - row;
//               j = Np - 1 - col;
//           } else{
//               i = row;
//               j = col;
//           }
          
          
//           double du[3];

//           force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

//           #pragma unroll(3)
//           for (int d = 0; d < 3; d++) {
//             U[d][i] += du[d];
//             U[d][j] -= du[d];
//           }
//       }
//   }
    
      #pragma omp parallel for schedule(static)
      for (int i = 0; i < Np; i++) {
            for (int j = i + 1; j < Np; j++) {
                  double du[3];

                  force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

                  #pragma unroll(3)
                  for (int d = 0; d < 3; d++) {
                    U[d][i] += du[d];
                    U[d][j] -= du[d];
                  }
            }
      }
}

// void
// verlet_step_accelerate (int Np, double dt, const double *restrict X[3], double *restrict U[3])
// {
//   for (int i = 0; i < Np; i++) {
//     for (int j = i + 1; j < Np; j++) {
//       double du[3];

//       force (dt, X[0][i], X[1][i], X[2][i], X[0][j], X[1][j], X[2][j], du);

//       for (int d = 0; d < 3; d++) {
//         U[d][i] += du[d];
//         U[d][j] -= du[d];
//       }
//     }
//   }
// }

