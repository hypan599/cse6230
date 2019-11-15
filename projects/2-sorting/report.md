# Project2 Report

## Changes we (plan to) make

### Qsort

- [ ] Use merge sort in each sort local (done)

- [x] Remove redundant redistribution (done)

- [x] Split comm before (need to check)

### Biotonic Sort

- [x] Change Ssend to Isend (talk about protocals and which fits this problem)

### General

- [ ] Choose to use which sort wisely (there may be sweet spots for bitonicsort)

## Results

| Uniform Size | Uniform Distribution | Partially Sorted | #Keys | Bandwidth(/1e9) |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 |
| 1 | 0 | 0 | 0 | 0 |
| 0 | 1 | 0 | 0 | 0 |
| 0 | 0 | 1 | 0 | 0 |
| 1 | 1 | 0 | 0 | 0 |
| 0 | 1 | 1 | 0 | 0 |
| 1 | 0 | 1 | 0 | 0 |
| 1 | 1 | 1 | 0 | 0 |


## Error
On 3 nodes with 28ppn:
```
[0] ./test_proj2 minKeys 160 maxKeys 40000 mult 32 seed 0 uniform size 1 uniform distribution 1 partially sorted 0
[0] Testing numKeysLocal 160, numKeysGlobal 13440, total bytes 107520
[0] Tested numKeysLocal 160, numKeysGlobal 13440, total bytes 107520: average bandwidth 9.327636e+07
[0] Testing numKeysLocal 5120, numKeysGlobal 430080, total bytes 3440640
[0] Tested numKeysLocal 5120, numKeysGlobal 430080, total bytes 3440640: average bandwidth 4.997941e+08
[0] Harmonic average bandwidth: 1.572123e+08
[mpiexec@rich133-k40-23-l.pace.gatech.edu] HYDT_bscd_pbs_wait_for_completion (../../../../src/pm/hydra/tools/bootstrap/external/pbs_wait.c:38): tm_poll(obit_event) failed with TM error 17001
[mpiexec@rich133-k40-23-l.pace.gatech.edu] HYDT_bsci_wait_for_completion (../../../../src/pm/hydra/tools/bootstrap/src/bsci_wait.c:23): launcher returned error waiting for completion
[mpiexec@rich133-k40-23-l.pace.gatech.edu] HYD_pmci_wait_for_completion (../../../../src/pm/hydra/pm/pmiserv/pmiserv_pmci.c:218): launcher returned error waiting for completion
[mpiexec@rich133-k40-23-l.pace.gatech.edu] main (../../../../src/pm/hydra/ui/mpich/mpiexec.c:340): process manager error waiting for completion
```
