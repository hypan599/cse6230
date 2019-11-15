#include <stdlib.h>
#include "proj2sorter.h"
#include "proj2sorter_impl.h"
#define SORT_NAME proj2_swenson
#define SORT_TYPE uint64_t
#include "swensonsort/sort.h"
#include <inttypes.h>

int Proj2SorterSortLocal_swenson_quick_sort(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  proj2_swenson_quick_sort(keys, numKeysLocal);
  if (direction == PROJ2SORT_BACKWARD)
  {
    for (int i = 0; i < numKeysLocal / 2; i++)
    {
      uint64_t swap = keys[i];
      keys[i] = keys[numKeysLocal - 1 - i];
      keys[numKeysLocal - 1 - i] = swap;
    }
  }
  return 0;
}

int Proj2SorterSortLocal_swenson_merge_sort(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  proj2_swenson_merge_sort(keys, numKeysLocal);
  if (direction == PROJ2SORT_BACKWARD)
  {
    for (int i = 0; i < numKeysLocal / 2; i++)
    {
      uint64_t swap = keys[i];
      keys[i] = keys[numKeysLocal - 1 - i];
      keys[numKeysLocal - 1 - i] = swap;
    }
  }
  return 0;
}

int smaller_than(uint64_t a, uint64_t b, int direction)
{
  return direction == PROJ2SORT_BACKWARD ? (a > b ? 1 : 0) : (a < b ? 1 : 0);
}

int Proj2SorterSortLocal_my_merge(Proj2Sorter sorter,
                                  size_t numKeysIn, uint64_t *keysIn,
                                  size_t numKeysIn2, uint64_t *keysOut,
                                  int direction)
{
  MPI_Comm comm = sorter->comm;
  int rank, i, err;
  int print_rank = 1;
  err = MPI_Comm_rank(comm, &rank);
  PROJ2CHK(err);
  // if (!rank)
  // {
  //   printf("At depth: %d, On rank 0: Before local sort:\n", flag);
  //   for (i = 0; i < numKeysLocal; i++)
  //   {
  //     printf("%" PRIu64 " ", keys[i]);
  //   }
  //   printf("\n");
  // }

  // One portion of the input is in keysIn at  0:numKeysIn
  // The other portion of the input is in keysOut at numKeysIn2:numKeysOut

  int numSorted = 0, left = 0, right = 0;
  while (numSorted < numKeysIn + numKeysIn2)
  {
    if (left == numKeysIn)
    {
      keysOut[numSorted] = keysOut[right + numKeysIn];
      right++;
    }
    else if (right == numKeysIn2)
    {
      keysOut[numSorted] = keysIn[left];
      left++;
    }
    else if (smaller_than(keysIn[left], keysOut[right + numKeysIn], direction) > 0)
    {
      keysOut[numSorted] = keysIn[left];
      left++;
    }
    else
    {
      keysOut[numSorted] = keysOut[right + numKeysIn];
      right++;
    }
    numSorted++;
  }

  // if (!rank)
  // {
  //   printf("At depth: %d, On rank 0: After local sort:\n", flag);
  //   for (i = 0; i < numKeysLocal; i++)
  //   {
  //     printf("%" PRIu64 " ", keys[i]);
  //   }
  //   printf("\n");
  // }

  return 0;
}

int Proj2SorterSortLocal_qsort(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  if (direction == PROJ2SORT_FORWARD)
  {
    qsort(keys, numKeysLocal, sizeof(*keys), uint64_comp_forward);
  }
  else
  {
    qsort(keys, numKeysLocal, sizeof(*keys), uint64_comp_backward);
  }
  return 0;
}

int Proj2SorterSortLocal(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  int err;
  err = Proj2SorterSortLocal_swenson_quick_sort(sorter, numKeysLocal, keys, direction);
  PROJ2CHK(err);

  return 0;
}
