# Paper Review

Paper: Fast, scalable and accurate finite-element based ab initio calculations using mixed precision computing: 46 PFLOPS simulation of a metallic dislocation system.

## a. What is the problem being solved

first principle calculation based on DFT (density fuctional theory) methods, using finite element method.

## b. What are the most important kernels of the algorithm for solving the problem? (In this context consider a kernel to be a subproblem that is relevant to more than just the specific problem from a.)

After a bunch of mathematical deduction, the authors have reduced the problem to:
main loop consists of dense matrix multiplication and matrix vector multiplication, in total `O(MlogM + 2MN + 4MN^2 + 2N^3)`, loop until result doesn't get better.

## c. What about the important kernels and/or the size of the problem make this a challenging problem

challenge in the linear algebra kernel is the large size of the matrix and high asymptotic complexity.

## d. Summarize the innovation of this paper

- First thing to mention is that the author applied finite element methods in DFT problem and reduced the problem to a set of linear problems to solve.
- The author managed to port most of the computation to GPU as wells as minimizing the data transfer between GPUs and CPU.
- Perform MPI communication for all wavefunction vectors in X_b simutaneously to reduce latency and overhead.
- Perform the most expensive computation in mixed precision to boost performance without reducing the final accuracy.

## e. What model is used that combines the problem parameters and machine parameters to predict performance

The author use strong scaling to demonstrate that the DFT-FE have higher speed up that previous state-of-art methods, and is much closer to ideal speed up. The author also used weak scaling to show that the program have a rather good parallelism efficiency on up to system with 20,000 electrons.

- In strong scaling, they use $T_f(512)$ (or $T_f(2048)$ etc. in other charts) as base case, and compared observed speed up with ideal speed up, and they get close to ideal speed up even with high number of MPI tasks. Here the model is simply $S_f(P) = \frac{T_f(1)}{T_f(P)}$.

- In weak scaling they choose to fix memory assiciated with the wavefuntion to a constant, and examine the performance. Here the models is $S_f(N, P) = \frac{T_f(N, 1)}{T_f(kN, k)}$, and they've chosen $T_f(2500, 54)$. Here N to P is not linear since the fixed constant is memory per MPI task, which is not necessarily linear to number of electrons.

## f. Any paper that is submitted for a prize contains some marketing, and maybe some attempts to fool the masses. If you could ask the authors to submit one additional figure with the performance measurements of an experiment, what would you choose, and why

- In section 7 the author only comparing the model with itself, which means that alghough the speed up form a good curve, the absolute runtime is unknown to us. The author did report runtime comparisons of time but that is done on smaller baseline problems, not on the large scale real problem. Thus I'd like to see a figure or table showing overall runtime (until solution converge) of the new method and current state-of-art method.