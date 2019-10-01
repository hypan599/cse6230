#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interactions.h"
#include "cloud_util.h"
#include "steric.h"
#include <omp.h>

typedef struct _box
{
  int head;
} box;

struct _ix
{
  double L;
  double r;
  int boxdim;
  box ***boxes;

#ifdef MULTIVECTOR
  int numContainers;
  int *curNx;
  int *maxNx;
  ix_pair **pairs;
#else
  int maxNx;
  int curNx;
  ix_pair *pairs;
#endif
};

// it is possible to use smaller boxes and more complex neighbor patterns
#define NUM_BOX_NEIGHBORS 13
int box_neighbors[NUM_BOX_NEIGHBORS][3] =
    {
        {-1, -1, -1},
        {-1, -1, 0},
        {-1, -1, +1},
        {-1, 0, -1},
        {-1, 0, 0},
        {-1, 0, +1},
        {-1, +1, -1},
        {-1, +1, 0},
        {-1, +1, +1},
        {0, -1, -1},
        {0, -1, 0},
        {0, -1, +1},
        {0, 0, -1}};

/* maxNx: _predicted_ maximum number of interactions */
int IXCreate(double L, int boxdim, int maxNx, IX *ix_p)
{
  IX ix;
  int err;

  if (boxdim < 4) /* need at least four boxes in each direction */
  {
    boxdim = 4;
  }
  err = safeMALLOC(sizeof(*ix), &ix);
  CHK(err);

  ix->L = L;
  ix->boxdim = boxdim;
  ix->curNx = 0;

#ifdef MULTIVECTOR
  ix->numContainers = omp_get_num_threads();
  err = safeMALLOC(ix->numContainers * sizeof(int), &(ix->curNx));
  CHK(err);
  err = safeMALLOC(ix->numContainers * sizeof(int), &(ix->maxNx));
  CHK(err);
  for (int i = 0; i < ix->numContainers; i++)
  {
    ix->maxNx[i] = maxNx;
    ix->curNx[i] = 0;
  }
  err = safeMALLOC(ix->numContainers * sizeof(ix_pair *), &(ix->pairs));
  CHK(err);
  for (int i = 0; i < ix->numContainers; i++)
  {
    err = safeMALLOC(maxNx * sizeof(ix_pair), &(ix->pairs[i]));
    CHK(err);
  }
  printf("CREATE %d pairs finish", ix->numContainers);
#else
  ix->curNx = 0;
  ix->maxNx = maxNx; // max value for each single container
  err = safeMALLOC(maxNx * sizeof(ix_pair), &(ix->pairs));
  CHK(err);
#endif
  err = safeMALLOC(boxdim * sizeof(box **), &(ix->boxes));
  CHK(err);
  for (int i = 0; i < boxdim; i++)
  {
    err = safeMALLOC(boxdim * sizeof(box *), &(ix->boxes[i]));
    CHK(err);
    for (int j = 0; j < boxdim; j++)
    {
      err = safeMALLOC(boxdim * sizeof(box), &(ix->boxes[i][j]));
      CHK(err);
    }
  }

  *ix_p = ix;
  return (0);
}

int IXDestroy(IX *ix_p)
{
  int boxdim = (*ix_p)->boxdim;
#ifdef MULTIVECTOR
  free((*ix_p)->curNx);
  free((*ix_p)->maxNx);
  for (int i = 0; i < (*ix_p)->numContainers; i++)
  {
    free((*ix_p)->pairs[i]);
  }
#endif
  free((*ix_p)->pairs);

  for (int i = 0; i < boxdim; i++)
  {
    for (int j = 0; j < boxdim; j++)
    {
      free((*ix_p)->boxes[i][j]);
    }
    free((*ix_p)->boxes[i]);
  }
  free((*ix_p)->boxes);
  free(*ix_p);
  *ix_p = NULL;
  return 0;
}

static void
IXClearPairs(IX ix)
{
#ifdef MULTIVECTOR
  for (int i = 0; i < ix->numContainers; i++)
  {
    ix->curNx[i] = 0;
  }
#else
  ix->curNx = 0;
#endif
}

static void
IXPushPair(IX ix, int p1, int p2)
{
  ix_pair *pair;
#ifdef MULTIVECTOR
  int current_thread = omp_get_thread_num();
  if (ix->curNx[current_thread] == ix->maxNx[current_thread])
  {
    int maxNx = ix->maxNx[current_thread] * 2;
    ix_pair *newpairs;

    safeMALLOC(maxNx * sizeof(ix_pair), &newpairs);
    memcpy(newpairs, ix->pairs[current_thread], ix->curNx[current_thread] * sizeof(ix_pair));
    free(ix->pairs[current_thread]);
    ix->pairs[current_thread] = newpairs;
    ix->maxNx[current_thread] = maxNx;
  }
  pair = &(ix->pairs[current_thread][(ix->curNx[current_thread])++]);
#else
  if (ix->curNx == ix->maxNx)
  {
    int maxNx = ix->maxNx * 2;
    ix_pair *newpairs;

    safeMALLOC(maxNx * sizeof(ix_pair), &newpairs);
    memcpy(newpairs, ix->pairs, ix->curNx * sizeof(ix_pair));
    free(ix->pairs);
    ix->pairs = newpairs;
    ix->maxNx = maxNx;
  }
  pair = &(ix->pairs[ix->curNx++]);
#endif
  pair->p[0] = p1;
  pair->p[1] = p2;
}

int interactions_check(IX ix, Vector X, double r, int Npairs, ix_pair *pairs, int *total)
{
  double L = ix->L;
  double r2 = r * r;
  int Np = X->Np;
  int intcount = 0;

  for (int i = 0; i < Np; i++)
  {
    for (int j = i + 1; j < Np; j++)
    {
      double dx, dy, dz;

      double dist2 = dist_and_disp(IDX(X, 0, i), IDX(X, 1, i), IDX(X, 2, i),
                                   IDX(X, 0, j), IDX(X, 1, j), IDX(X, 2, j), L,
                                   &dx, &dy, &dz);
      if (dist2 < r2)
      {
        intcount++;
        int k;
        for (k = 0; k < Npairs; k++)
        {
          if ((pairs[k].p[0] == i && pairs[k].p[1] == j) ||
              (pairs[k].p[0] == j && pairs[k].p[1] == i))
          {
            break;
          }
        }
        if (k == Npairs)
        {
          fprintf(stderr, "Pair %d %d not in list\n", i, j);
          return 1;
        }
      }
    }
  }
  *total = intcount;
  return 0;
}

int IXGetPairs(IX ix, Vector X, double r, int *Npairs, ix_pair **pairs)
{
  int boxdim = ix->boxdim;
  double L = ix->L;
  double boxwidth = L / boxdim;
  double cutoff2 = r * r;
  box ***b = ix->boxes;
  int Np = X->Np;
  int err;

  if (r > boxwidth)
  {
    printf("interactions: radius %g is greater than box width %g\n", r, boxwidth);
    return 1;
  }

  // box indices
  int idx, idy, idz;
  int neigh_idx, neigh_idy, neigh_idz;
  int *next;
  box *bp, *neigh_bp;

  for (int i = 0; i < boxdim; i++)
  {
    for (int j = 0; j < boxdim; j++)
    {
      for (int k = 0; k < boxdim; k++)
      {
        b[i][j][k].head = -1;
      }
    }
  }

  err = safeMALLOC(Np * sizeof(int), &next);
  CHK(err);

  // traverse all particles and assign to boxes
  for (int i = 0; i < Np; i++)
  {
    double pos_p[3];
    // initialize entry of implied linked list
    next[i] = -1;

    // get the periodic coordinates in [0,L)
    for (int j = 0; j < 3; j++)
    {
      //printf("%g\n",IDX(X,j,i));
      pos_p[j] = force_remainder(IDX(X, j, i), L) + L / 2.;
    }
    // which box does the particle belong to?
    idx = (int)(pos_p[0] / L * boxdim);
    idy = (int)(pos_p[1] / L * boxdim);
    idz = (int)(pos_p[2] / L * boxdim);

    // add to beginning of implied linked list
    bp = &b[idx][idy][idz];
    next[i] = bp->head;
    bp->head = i;
  }

  int p1, p2;
  double d2, dx, dy, dz;

  IXClearPairs(ix);
#pragma omp parallel for schedule(static) private(p1, p2)
  for (idx = 0; idx < boxdim; idx++)
  {
    for (idy = 0; idy < boxdim; idy++)
    {
      for (idz = 0; idz < boxdim; idz++)
      {
        bp = &b[idx][idy][idz];

        // within box interactions
        p1 = bp->head;
        while (p1 != -1)
        {
          p2 = next[p1];
          while (p2 != -1)
          {
            d2 = dist_and_disp(IDX(X, 0, p1), IDX(X, 1, p1), IDX(X, 2, p1),
                               IDX(X, 0, p2), IDX(X, 1, p2), IDX(X, 2, p2), L,
                               &dx, &dy, &dz);

            if (d2 < cutoff2)
            {
              IXPushPair(ix, p1, p2);
            }

            p2 = next[p2];
          }
          p1 = next[p1];
        }

        // interactions with other boxes
        for (int j = 0; j < NUM_BOX_NEIGHBORS; j++)
        {
          neigh_idx = (idx + box_neighbors[j][0] + boxdim) % boxdim;
          neigh_idy = (idy + box_neighbors[j][1] + boxdim) % boxdim;
          neigh_idz = (idz + box_neighbors[j][2] + boxdim) % boxdim;

          neigh_bp = &b[neigh_idx][neigh_idy][neigh_idz];

          p1 = neigh_bp->head;
          while (p1 != -1)
          {
            p2 = bp->head;
            while (p2 != -1)
            {
              d2 = dist_and_disp(IDX(X, 0, p1), IDX(X, 1, p1), IDX(X, 2, p1),
                                 IDX(X, 0, p2), IDX(X, 1, p2), IDX(X, 2, p2), L,
                                 &dx, &dy, &dz);

              if (d2 < cutoff2)
              {
                IXPushPair(ix, p1, p2);
              }

              p2 = next[p2];
            }
            p1 = next[p1];
          }
        }
      }
    }
  }
  // printf("GET PAIRS done, now merge results");
  free(next);
#ifdef MULTIVECTOR
  // have to join result of all pairs here
  int totalNumPairs = 0;
  for (int i = 0; i < ix->numContainers; i++)
  {
    totalNumPairs += ix->curNx[i];
  }
  *Npairs = totalNumPairs;
  ix_pair *totalPairs;
  err = safeMALLOC(totalNumPairs * sizeof(ix_pair), &totalPairs);
  CHK(err);
  int tmp = 0;
  for (int i = 0; i < ix->numContainers; i++)
  {
    for (int j = 0; j < ix->curNx[i]; j++)
    {
      totalPairs[tmp + j] = ix->pairs[i][j];
    }
    tmp += ix->curNx[i];
  }
  // *Npairs = tmp;
  *pairs = totalPairs;
#else
  *Npairs = ix->curNx;
  *pairs = ix->pairs;
#endif

#if DEBUG
  {
    int NpairsCheck;

    err = interactions_check(ix, X, r, *Npairs, *pairs, &NpairsCheck);
    CHK(err);

    printf("Npairs %d, NpairsCheck %d\n", *Npairs, NpairsCheck);

    if (*Npairs != NpairsCheck)
    {
      fprintf(stderr, "Interaction count mismatch, %d != %d\n", *Npairs, NpairsCheck);
    }
  }
#endif

  return 0;
}

int IXRestorePairs(IX ix, Vector X, double r, int *Npairs, ix_pair **pairs)
{
  *Npairs = 0;
  *pairs = NULL;
  return 0;
}
