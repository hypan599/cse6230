# Project2 Report

## Changes we (plan to) make

### Qsort

- [x] Use merge sort in each sort local (done)

- [x] Remove redundant redistribution (done)

- [x] Split comm before (need to check)

### Biotonic Sort

- [x] Change Ssend to Isend (talk about protocals and which fits this problem)

### Note

All tests are executed by the following command:
```bash
mpirun 
```

### Report

To simplify the testing procedure, the initial version of codes and all modifications are compared in the running condition shown below

160 400000 32 0 5 1 1 0

4 Nodes are used during the testing process.

The picture below is the composition of time used by all threads (including the attached hpc threads) for running the initial codes.

![](Test13_noaction_Figure.PNG)

The picture below is the composition of time used by One thread for running initial version of codes.

![](Test13_noaction_manysplit.PNG)

Firstly, what we can find from the figure above is that in every iteration there is a PMPI_Comm_Split process. Since the method of dividing the threads is known and does not change in different computations, building all needed communicators and dividing threads into correct communicators can finish before the beginning of sorting.

All splitting process is done during the process of creating the Sorter.

The figure below shows the composition of time used by One thread for running codes putting all splitting together before sorting.

![Test10_no_split](./Test10_no_split.PNG)

Comparing with the first figure, the time occupied by PMPI_Comm_split decreased so much that it is not easy to find it.

Secondly, although it is not an independent function, the redistribution of the sorted arrays is executed in all iterations of quichsort function. In fact, the sorted arrays does not need to be redistributed immediately. The sorted lists can be sent back directly at the end of the iteration and redistributed at the last iteration.

The modification of this part, start from the original codes, divides the redistribute part as a independent function.

The figure below shows the composition of time used by One thread for running codes redistributing the sorted arrays at the last iteration.

![Test12_lastredistribute](./Test12_lastredistribute.PNG)

Since in the initial code this part is not independent, it is not easy to compare the improvement of the modification with the initial code by looking at the figures; however, the slightly increase of proportion of the finalize part in the whole running time still shows that the time of sorting decreases because of the modification.

Thirdly, the method of sorting in every iteration used in the initial codes is quicksort. However, after the first random distribution, all arrays waiting for being sorted have two or three ranges of increasing. In this situation, merge sort is more effective.

In the modification from the original codes, the arrays are sorted by merge sort ONCE a transport-merge of array happens. This is to prevent the possible trouble of sorting arrays having three ranges of increasing (such thread receives sorted arrays from its partner and the last thread when the size of the communicator is odd).

The picture below is the composition of time used by all threads (including the attached hpc threads) for running the modified codes using merge sort.

![Test14Figure](./Test14Figure.PNG)

It is obvious that merge sort reduces the time of computation substantially.

The figure below shows the composition of time used by One thread for running codes using merge sort.

![Test14OneThread](./Test14OneThread.PNG)

Last but not least, the communication method between threads may also influence the total time of running the codes. In some places, using MPI_Isend is better for the reason that the thread sending messages may not be blocked by the thread receiving process. It is also modified in the code.

The new codes are obtained by adding all of the modifications mentioned above together. The picture below is the composition of time used by all threads (including the attached hpc threads) for running the codes having all modifications.

![Test15Figure](./Test15Figure.PNG)

The figure below shows the composition of time used by One thread for running codes having all modifications.

![Test15OneThread](./Test15OneThread.PNG)
