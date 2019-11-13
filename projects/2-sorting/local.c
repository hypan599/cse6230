#include <stdlib.h>
#include "proj2sorter.h"
#include "proj2sorter_impl.h"
#define SORT_NAME proj2_swenson
#define SORT_TYPE uint64_t
#include "swensonsort/sort.h"
#include <stdbool.h>

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

bool smaller_than(uint64_t a, uint64_t b, int direction)
{
  return direction == PROJ2SORT_BACKWARD ? (a > b) : (a < b);
}

int Proj2SorterSortLocal_my_merge_sort(Proj2Sorter sorter, size_t numKeysLocal, uint64_t *keys, int direction)
{
  int right = 0;
  for (int i = 1; i < numKeysLocal; i++)
  {
    if (smaller_than(keys[i], keys[i - 1], direction))
    {
      right = i;
      break;
    }
  }
  int numSorted = 0, left = 0;
  uint64_t *tmp_result = (uint64_t *)malloc(sizeof(uint64_t) * numKeysLocal);
  while (numSorted < numKeysLocal)
  {
    if (smaller_than(keys[left], keys[right], direction))
    {
      tmp_result[numSorted] = keys[left++];
    }
    else
    {
      tmp_result[numSorted] = keys[right++];
    }
    numSorted++;
  }
  memcpy(keys, tmp_result, numSorted * sizeof(*keys));
  free(tmp_result);
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
  if (flag)
  {
    err = Proj2SorterSortLocal_my_merge_sort(sorter, numKeysLocal, keys, direction);
    PROJ2CHK(err);
  }
  else
  {
    err = Proj2SorterSortLocal_swenson_quick_sort(sorter, numKeysLocal, keys, direction);
    PROJ2CHK(err);
  }
  return 0;
}
