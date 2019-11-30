# Paper Review

Paper: Fast, scalable and accurate finite-element based ab initio calculations using mixed precision computing: 46 PFLOPS simulation of a metallic dislocation system.

## a. What is the problem being solved

first principle calculation based on DFT (density fuctional theory) methods on metallic system

## b. What are the most important kernels of the algorithm for solving the problem? (In this context consider a kernel to be a subproblem that is relevant to more than just the specific problem from a.)

the kernel in this problem is a PDE solver (finite-element based)

## c. What about the important kernels and/or the size of the problem make this a challenging problem

Existing FE challenges

- **c.1** large number of DoF(degree of freedom)
- **c.2** GHEP (generalized hermitian eigenvalue problem) is harder than SHEP (standard hermitian eigenvalue problem)

## d. Summarize the innovation of this paper

- d.1: development of efficient and accurate spatially adaptive discretization strategies using higher order finite element discretization
- d.2: efficient and scalable algorigthms in conjunction with mixed precision strategy for the solution of kohn sham equations
- d.3: implementation innovation that reduce the data movement costs and increase arithmetic intensity

Namely, in d.1 and d.2 the author solved c.1 by employing *error-analysis informed adaptive higher order FE discretization* that reduce DoF, and c.2 by employing finite-elements with nodal points coincident with Gauss-Lobatto-Legendre points.

In d.3 the author took advantage of the fact that in matrix multiplication, all wavefuntion vectors have same p2p communication pattern, and thus they could be combined together to avoid paying redudant latency. The author also employed FE cell level dense matrix operations instead of global sparse matrix operations, as well as porting all computational intensive operations to GPU.

## e. What model is used that combines the problem parameters and machine parameters to predict performance?

- **System and environment**: Summit supercomputer(CPU-GPU), Theta and Cori supercomputer(CPU)
- **Application**: pyramidal II screw dislocation in Mg
- **Measurement**: stable single SCF iteration time. `MPI_Wtime` for CPU; `MPI_Wtime` + `cudaDeviceSynchronize` for CPU-GPU. FLOP measured by `nvprof`, take avg of 2 MPI tasks, then scale to full size.

- **Model**: strong scaling and weak scaling

## f. Any paper that is submitted for a prize contains some marketing, and maybe some attempts to fool the masses. If you could ask the authors to submit one additional figure with the performance measurements of an experiment, what would you choose, and why?
