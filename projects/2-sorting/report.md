# Project2 Report

## Major Changes and Result

Firstly we run the original test program with hpcviewer to get a general idea about the code, and use this as baseline for optimizations.

The picture below is the composition of time used by all threads for running the initial codes. (Note that we didn't hide threads owned by MPI and this information is useful later)

![](Test13_noaction_Figure.PNG)

The picture below is the composition of time used by One thread for running initial version of codes.

![](Test13_noaction_manysplit.PNG)

Note that there is 5 runs of sort program, each with different number of keys, and thus the trace show a peridocal pattern.

From reading and analyzing source code of qsort and bitonic sort, we found several places that we can plugin some optimizations. Algorithmically, there is redundant qsortLocal redistribution called, and there is use cases of non-optimal MPI communication patterns. Each of them are described below.

### Result

The picture below is the composition of time used by all threads for running the codes having all optimization.
![Test15Figure](./Test15Figure.PNG)

It can be easily seen that the first part are "virtual" threads created by MPI runtime, and the time of MPI initialization shouldn't vary too much. We can clearly see the speed up of sorting program by comparing with time cost of MPI initialization. The speed up is roughly 6 (1.5 * 4) times.

The figure below shows the composition of time used by One thread for running codes having all modifications.
![Test15OneThread](./Test15OneThread.PNG)

### Split comm ahead

In every iteration there is a PMPI_Comm_Split process. Since the group of subcommunicators doesn't depend on input data, we can put it in the initialization of sorter object. Note that same subcomm can be used in each run, and instantiation of sorter object is not timed!

The figure below shows the composition of time used by one thread for running codes putting all splitting together before sorting. Note that `PMPI_Comm_Split` only disappear at the beginning of program run and called before recursive call of qsort starts.
![Test10_no_split](./Test10_no_split.PNG)

### Remove redundant redistribution

Although not visible in the trace viewer, the redistribution of the sorted array is executed in all iterations of quichsort function, which is redundant. We only need to do this redistribution at the last iteration to return correct result, since uneven number of keys doesn't break the sort.

The optimization here including extract the redistribute part into a new function.

The figure below shows the composition of time used by one thread for running codes redistributing the sorted arrays at the last iteration.

![Test12_lastredistribute](./Test12_lastredistribute.PNG)

However, the slightly increase of proportion of the finalize part in the whole running time shows that we are doing what we expect since expectation of number of keys redistributed is related to number of keys from input and distribution of them. If we do redistribution within recursive call, the difference among ranks are smoothed each time, and if we do this only once in the end, the expected number of keys gets send should be bigger.

Since in the initial code this part is not independent, it is not easy to compare the improvement of the modification with the initial code by looking at the trace view, the speed up can only be proved by increase of bandwidth indirectly.

### Use merge sort in each sort local

In the qsort, each rank is receiving both lower (upper) half of sorted arrays from itself and its peer, and then a local quick sort is done on the received array. Not only is quick sort reaching its theoretical worst case, but also we are doing redundant sort if the input is two monotonic sub-arrays. In this case, a "merge" would be enough to give use a sorted result given such input. One trick here is to do the "merge" right after any `MPI_Recv` call to avoid have more than 2 monotonic components.

Note that this "merge" reduce the asymptotic worst case lower bound to `O(n)` with a small constant factor!

The figure below shows the composition of time used by One thread for running codes using optimized merge.

![Test14OneThread](./Test14OneThread.PNG)

The picture below is the composition of time used by all threads for running the modified codes using optimized merge.

![Test14Figure](./Test14Figure.PNG)

Note: This work is build on top of "reducing redundant redistribution".

### Change Ssend to Isend

In bitonic sort, as we are only sending one value, with a point to point manner, it is safe to say that there is enough space for MPI to create internal buffer for messages. Thus we will want to replace `Ssend` with `Isend` since by doing synchronous blocking send we are doubling latency with same small message, which is nearly double of communication time.

Since this is easy to prove and not easy to visualize in hpcviewer, we don't have figure for this change. I do remember seeing significant increase in bandwidth but since we didn't continue to optimize bitonic sort for other reason, I'll simply leave it there.

## Note

All tests are executed by the following command with nodes=4:ppn=28

```bash
mpirun  -f ${PBS_NODEFILE} -n ${PBS_NP} ./test_proj2 160 400000 32 0 5 1 1 0
```

Note that although we did optimize bitonic sort, since the code is tested and graded on n=112, and we did enough optimization on qsort, bitonic sort is not used at all.
Also, we are not optimizing when partially_sorted=1 sine its not the baseline case and we don't have extra time for that.

Git-branching: I have different branch named `hw7_*` for each single idea of optimization, and then the are merged together to create final result. There may be unfinalized work in individual branch due to lack of time, but the merge history should be clear enough.
