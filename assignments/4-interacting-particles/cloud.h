#if !defined(CLOUD_H)
#define      CLOUD_H
#include <math.h>
#include <cse6230rand.h>

/* a quadratic modification of periodic distance,
 * with a periodicity of 1 */
#define qdist(x) ((x) * (1. - fabs(x)))

/* The derivative of qdist wrt x */
#define dqdist(x) (1. - 2. * fabs(x))

#pragma omp declare simd
static inline double
my_remainder(double x, double y)
{
  double d = x / y;
  double n = rint(d);
  return x - n * y;
}

#if defined(MY_REMAINDER) && MY_REMAINDER
#define force_remainder my_remainder
#else
#define force_remainder remainder
#endif

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

  dx = force_remainder(x1 - x2, 1.);
  dy = force_remainder(y1 - y2, 1.);
  dz = force_remainder(z1 - z2, 1.);

  dx = qdist(dx);
  dy = qdist(dy);
  dz = qdist(dz);

  r2 = dx*dx + dy*dy + dz*dz;
  ir = 1. / sqrt(r2);

  return k * ir;
}



#if !defined(DUMMY) || !DUMMY
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

  dx = force_remainder(x1 - x2, 1.);
  dy = force_remainder(y1 - y2, 1.);
  dz = force_remainder(z1 - z2, 1.);

  qdx = qdist(dx);
  qdy = qdist(dy);
  qdz = qdist(dz);

  r2 = qdx*qdx + qdy*qdy + qdz*qdz;
  ir3 = 1. / (sqrt(r2) * r2);

  f[0] = k * ir3 * qdx * dqdist(dx);
  f[1] = k * ir3 * qdy * dqdist(dy);
  f[2] = k * ir3 * qdz * dqdist(dz);
}

static inline void
force_2 (double k,
       double x1, double y1, double z1,
       double x2, double y2, double z2,
       double *fx, double *fy, double *fz)
{
  double dx, dy, dz;
  double qdx, qdy, qdz;
  double r2, ir3;

  dx = force_remainder(x1 - x2, 1.);
  dy = force_remainder(y1 - y2, 1.);
  dz = force_remainder(z1 - z2, 1.);

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
force_vec (int K, double k,
       double x1, double y1, double z1,
       const double *x2, const double *y2, const double *z2,
       double *fx, double *fy, double *fz)
{
  #pragma omp simd
  for (int i = 0; i < K; i++) {
    double dx, dy, dz;
    double qdx, qdy, qdz;
    double r2, ir3;
    dx = force_remainder(x1 - x2[i], 1.);
    dy = force_remainder(y1 - y2[i], 1.);
    dz = force_remainder(z1 - z2[i], 1.);

    qdx = qdist(dx);
    qdy = qdist(dy);
    qdz = qdist(dz);

    r2 = qdx*qdx + qdy*qdy + qdz*qdz;
    ir3 = 1. / (sqrt(r2) * r2);

    fx[i] = k * ir3 * qdx * dqdist(dx);
    fy[i] = k * ir3 * qdy * dqdist(dy);
    fz[i] = k * ir3 * qdz * dqdist(dz);
  }
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

static inline void
force_2 (double k,
         double x1, double y1, double z1,
         double x2, double y2, double z2,
         double *fx, double *fy, double *fz)
{
  *fx = 1.e-6 + y1 * z2 - y2 * z1;
  *fy = -1.e-6 + z1 * x2 - z2 * x1;
  *fz = -5.e-7 + x1 * y2 - x2 * y1;
}

static inline void
force_vec (int K, double k,
           double x1, double y1, double z1,
           double *x2, double *y2, double *z2,
           double *fx, double *fy, double *fz)
{
  for (int i = 0; i < K; i++) {
    fx[i] = 1.e-6  + y1 * z2[i] - y2[i] * z1;
    fy[i] = -1.e-6 + z1 * x2[i] - z2[i] * x1;
    fz[i] = -5.e-7 + x1 * y2[i] - x2[i] * y1;
  }
}
#endif


void initialize_variables (int Np, double k, cse6230rand_t *rand, double *X0[3], double *X[3], double *U[3]);
double compute_hamiltonian (int Np, double k, const double *X[3], const double *U[3]);

#endif
