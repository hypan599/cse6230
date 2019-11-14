# Project2 Report

## Changes we (plan to) make

### Qsort

- [x] Use merge sort in each sort local (done)

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