#if !defined(CLOUD_H)
#define      CLOUD_H
#include <math.h>
#include <cse6230rand.h>

/* a quadratic modification of periodic distance,
 * with a periodicity of 1 */
#define qdist(x) ((x) * (1. - fabs(x)))

/* The derivative of qdist wrt x */
#define dqdist(x) (1. - 2. * fabs(x))

/* computes the potential between two particles,
 * assuming they are moving in a periodic domain.
 * This model problem asssumes both masses are 1.
 * To make the potential smooth, we smooth out the
 * elbow in the periodic distance function with
 * a quadratic approximation */
static inline double
potential (double k,
           double x1, double y1, double z1,
           double x2, double y2, double z2)
{
  double dx, dy, dz;
  double r2, ir;

  dx = remainder(x1 - x2, 1.);
  dy = remainder(y1 - y2, 1.);
  dz = remainder(z1 - z2, 1.);

  dx = qdist(dx);
  dy = qdist(dy);
  dz = qdist(dz);

  r2 = dx*dx + dy*dy + dz*dz;
  ir = 1. / sqrt(r2);

  return k * ir;
}

#if !defined(DUMMY)
/* computes the force particle 2 exerts on particle 1,
 * according to the potential above */
static inline void
force (double k,
       double x1, double y1, double z1,
       double x2, double y2, double z2,
       double f[])
{
  double dx, dy, dz;
  double qdx, qdy, qdz;
  double r2, ir3;

  dx = remainder(x1 - x2, 1.);
  dy = remainder(y1 - y2, 1.);
  dz = remainder(z1 - z2, 1.);

  qdx = qdist(dx);
  qdy = qdist(dy);
  qdz = qdist(dz);

  r2 = qdx*qdx + qdy*qdy + qdz*qdz;
  ir3 = 1. / (sqrt(r2) * r2);

  f[0] = k * ir3 * qdx * dqdist(dx);
  f[1] = k * ir3 * qdy * dqdist(dy);
  f[2] = k * ir3 * qdz * dqdist(dz);
}
#else
static inline void
force (double k,
       double x1, double y1, double z1,
       double x2, double y2, double z2,
       double f[])
{
  f[0] = 1.e-6 + y1 * z2 - y2 * z1;
  f[1] = -1.e-6 + z1 * x2 - z2 * x1;
  f[2] = -5.e-7 + x1 * y2 - x2 * y1;
}
#endif

static inline void
force_2 (double k,
       double x1, double y1, double z1,
       double x2, double y2, double z2,
       double *fx, double *fy, double *fz)
{
  double dx, dy, dz;
  double qdx, qdy, qdz;
  double r2, ir3;

  dx = remainder(x1 - x2, 1.);
  dy = remainder(y1 - y2, 1.);
  dz = remainder(z1 - z2, 1.);

  qdx = qdist(dx);
  qdy = qdist(dy);
  qdz = qdist(dz);

  r2 = qdx*qdx + qdy*qdy + qdz*qdz;
  ir3 = 1. / (sqrt(r2) * r2);

  *fx = k * ir3 * qdx * dqdist(dx);
  *fy = k * ir3 * qdy * dqdist(dy);
  *fz = k * ir3 * qdz * dqdist(dz);
}

static inline void
force_vec (double K, double k,
       double x1, double y1, double z1,
       double *restrict x2, double *restrict y2, double *restrict z2,
       double *restrict fx, double *restrict fy, double *restrict fz)
{
#define VMAX 32
  double dx[VMAX], dy[VMAX], dz[VMAX];
  double qdx[VMAX], qdy[VMAX], qdz[VMAX];
  double r2[VMAX], ir3[VMAX];

  for (int i = 0; i < K; i++) {
    dx[i] = remainder(x1 - x2[i], 1.);
    dy[i] = remainder(y1 - y2[i], 1.);
    dz[i] = remainder(z1 - z2[i], 1.);

    qdx[i] = qdist(dx[i]);
    qdy[i] = qdist(dy[i]);
    qdz[i] = qdist(dz[i]);

    r2[i] = qdx[i]*qdx[i] + qdy[i]*qdy[i] + qdz[i]*qdz[i];
    ir3[i] = 1. / (sqrt(r2[i]) * r2[i]);

    fx[i] = k * ir3[i] * qdx[i] * dqdist(dx[i]);
    fy[i] = k * ir3[i] * qdy[i] * dqdist(dy[i]);
    fz[i] = k * ir3[i] * qdz[i] * dqdist(dz[i]);
  }
}

void initialize_variables (int Np, double k, cse6230rand_t *rand, double *X0[3], double *X[3], double *U[3]);
double compute_hamiltonian (int Np, double k, const double *X[3], const double *U[3]);

#endif
