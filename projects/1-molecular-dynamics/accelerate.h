#if !defined(ACCELERATE_H)
#define      ACCELERATE_H

#include "vector.h"

typedef struct _accel_t *Accel;

int AccelCreate(int Np, double L, double k, double r, int use_ix, Accel *accel);
int AccelDestroy(Accel *accel);

void
accelerate (Accel accel, Vector X, Vector V, int control_update); //add the coefficient of period update_inter_pair 

#endif
