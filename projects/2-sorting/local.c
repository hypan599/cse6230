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

int Proj2SorterSortLocal_my_merge(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  MPI_Comm comm = sorter->comm;
  int rank, i, err;
  int print_rank = 1;
  err = MPI_Comm_rank(comm, &rank);
  PROJ2CHK(err);
  if (rank == print_rank)
  {
    printf("On rank 0: Before local sort:\n");
    for (i = 0; i < numKeysLocal; i++)
    {
      printf("%" PRIu64 " ", keys[i]);
    }
    printf("\n");
  }
  int right = 0, numSorted = 0, left = 0;
  for (int i = 1; i < numKeysLocal; i++)
  {
    if (smaller_than(keys[i], keys[i - 1], direction) > 0)
    {
      right = i;
      break;
    }
  }
//   right ++;
  int right_save = right;
  if (right == 0)
    return 0;
  if (rank == print_rank) {
      printf("Left: %d, Right: %d, Total: %d\n", left, right, (int)numKeysLocal);
  }
  uint64_t *tmp_result = (uint64_t *)malloc(sizeof(uint64_t) * numKeysLocal);
  while (numSorted < numKeysLocal)
  {
    if (left == right_save)
    {
      tmp_result[numSorted] = keys[right++];
          if (rank == print_rank) {
              printf("r");
          }
    }
    else if (right == numKeysLocal)
    {
      tmp_result[numSorted] = keys[left++];
          if (rank == print_rank) {
              printf("l");
          }
    }
    else if (smaller_than(keys[left], keys[right], direction) > 0)
    {
      tmp_result[numSorted] = keys[left++];
          if (rank == print_rank) {
              printf("l");
          }
    }
    else
    {
      tmp_result[numSorted] = keys[right++];
          if (rank == print_rank) {
              printf("r");
          }
    }
    numSorted++;
  }
  if (rank == print_rank) {
      printf("\n");
  }
  
  memcpy(keys, tmp_result, numKeysLocal * sizeof(uint64_t));
  free(tmp_result);

  if (rank == print_rank)
  {
    printf("On rank 0: After local sort:\n");
    for (i = 0; i < numKeysLocal; i++)
    {
      printf("%" PRIu64 " ", keys[i]);
    }
    printf("\n");
  }

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

int Proj2SorterSortLocal(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction, int flag)
{
  int err;

  if (flag > 0)
  {
    err = Proj2SorterSortLocal_swenson_merge_sort(sorter, numKeysLocal, keys, direction);
    PROJ2CHK(err);
  }
  else
  {
    err = Proj2SorterSortLocal_swenson_quick_sort(sorter, numKeysLocal, keys, direction);
    PROJ2CHK(err);
  }

  return 0;
}
