# Paper Review

Paper: Fast, scalable and accurate finite-element based ab initio calculations using mixed precision computing: 46 PFLOPS simulation of a metallic dislocation system.

### a. What is the problem being solved?

first principle calculation based on DFT (density fuctional theory) methods on metallic system

### b. What are the most important kernels of the algorithm for solving the problem? (In this context consider a kernel to be a subproblem that is relevant to more than just the specific problem from a.)

FE

### c. What about the important kernels and/or the size of the problem make this a challenging problem?

existing FE challenges: 
- **c.1** large number of DoF(degree of freedom)
    - sth here
- **c.2** GHEP (generalized hermitian eigenvalue problem) is harder than SHEP (standard hermitian eigenvalue problem)

### d. Summarize the innovation of this paper.

employ FE discretization of kohn-sham equation. localized with compact support, having smaller communication cost.

### e. What model is used that combines the problem parameters and machine parameters to predict performance?

### f. Any paper that is submitted for a prize contains some marketing, and maybe some attempts to fool the masses. If you could ask the authors to submit one additional figure with the performance measurements of an experiment, what would you choose, and why?
