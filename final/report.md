# Benchmark Report

## 1.1 (1 pts): In a cell below, tell me which benchmark you are choosing. Provide a link. If the benchmark is actually a suite of benchmarks, tell me which one you would like to focus on. If there are citations for the benchmark, give me those, too, please. After that, give:

We are going to choose [HPLinpack](http://www.netlib.org/benchmark/hpl/).

- As complete a description as possible of the problem being solved. Include scaling parameters like problem size $N$, and any other "free" parameters that can change between different runs of the benchmark.

HPL is trying to solve a dense linear system.

- As complete a description as possible of the value of the benchmark: what quantity is being reported?

HPL reports Flops reached in th progress.

- Then, tell me which type of pace-ice node you intend to use to test the benchmark.

We plan to use nodes with 28 core CPUs.
We will focus on first get this thing running on CPU, then maybe try utilize GPU.

## 1.2 (4 pts): In your own words, give me your assessment of the quality of the benchmark according to the four points (a), (b), (c), and (d) above.

- a. Describe some applications where the benchmark problem is relevant. Benchmarks must walk a fine line between being to specific to one application but very predictive, versus being general to lots of applications while being too simple to predict the performance of any application very well. Do you think the benchmark you chose does a good job with this balance?

- b. What assumptions does your benchmark make about the kind of machine that it is run on? Do you think that those assumptions are reasonable? Let's make this question very concrete: let's say you have access to TaihuLight, whose nodes are neither really CPUs or GPUs, but somewhere in between. Could your benchmark run on this machine? If not, propose a way that you could change the benchmark to make it more portable.

The HPL package doesn't make any assumption on the platform, since HPL is only calling any linear algebra routines provided by user. The only thing here is that user is responsible for providing a MPI and linear algebra implementation.

- c. How exactly does your benchmark specify the way the problem is solved? If your benchmark is for a particular algorithm or a particular code, do you think that the results of the benchmark would help you predict the performance of a different code/algorithm solving the same problem on the same machine?

- d. One measure of the complexity of a benchmark is how difficult it would be to write a reference implementation from scratch (one that solves the problem, if not in a "high-performance" way). If you had to guess, how big would a team have to be do that: (i) one dedicated programmer; (ii) a team of about a dozen (like a research lab); (iii) an Organization (like a division of a company or a government agency)? Give your reasoning (by, e.g. measuring lines of code in the implementation you will be working with)

Total lines of code in hpl: 12009

In assignments 6, we did matvec using MPI, given skeleton code with 857 lines. And after plenty of work we finally come up with a working version consisting of 1042 lines in one week. Here we observe: 2 dedicated student, 7 days, ~150 lines of code. Given the fact that we spent some time working on other courses, and we did spend some time debugging pointers, and assume experienced programmer doesn't make this type of mistakes at all, we speed up 200% resulting in himself writing ~300 lines of code per week. Thus he needs 400 weeks to finish the project if he can stay in this pace.

Given a dozen prople, we can speed up roughly 10-15 times resulting in less that 40 weeks, which is about 3 quarters.

Given a company, let's say we have a product manager and he assigns different part of the program to different teams, we can speed up at least 20 times. Generally a company should be able to finish that work in 2-3 month.

Note: in all counting above, comments and blank lines in source files are ignored.

### 1.3 (1 pts): Try to prepare for some of the logistics ahead of you. Answer the following questions:

- a. Where / how will you obtain the source for the benchmark driver and implementation that you will be using? (Regarding how: is it a tarball, repository, or other?)

Source code is downloaded from link above.

- b. What software environments will you need to build and run the benchmark? (e.g. Does it use raw make? Autotools? CMake? Is it python/pip/conda? Does it need MPI? OpenMP? Cuda?)

Dependency:

- [Intel MKL](https://software.intel.com/en-us/mkl)
- [MPI](https://www.open-mpi.org/)

Installation:

To install MKL, download source tarball and run `install.sh`.

To install MPI follow official [documentation](https://www.open-mpi.org/faq/?category=building). Here MPI is shipped with pace module.

To install HPL, download source tarball and follow the `INSTALL` file. Remember to change make file to point those environment variables to correct mpi and mkl installation.

### 1.4 (1 pts): Successfully install and run your benchmark

Include in this directory an example job submission script that runs your benchmark code.

job submission script: `hpl_run.pbs`

HPL parameters:

### 1.5 (3 pts): Develop a performance model for your benchmark

In 1.1, you chose a performance metric of your benchmark, let's call it $V$. Your benchmark will solve a problem with some parameters (problem size, the choice of matrix / network / etc.), let's call those parameters $N$. The node that you chose to run on will have some machine parameters (The number of cores, the type of GPU, the bandwidth from main memory, etc., etc.), let's call them $P$. Give an expression for $V(N,P)$ for your benchmark, and describe how you arrived at it. You should use your discretion when choosing the level of detail. If it is hard to develop a closed-form performance model for the whole benchmark, but there are a few key kernels that happens repeatedly in your benchmark (a stencil application, an iteration of stochastic gradient descent, etc.), you can give performance models for those benchmarks(s) instead.

If it is difficult to formulate your expression in terms of machine parameters, try to develop an expression with coefficients that measure the rates at which the machine can do some lower-level operations (for example, the rate at which a GPU can sum an array). If you have these coefficients, you should give a plausible description for how the architecture of the machine affects those rates.

### 1.6 (2 pts): Gather statistics for the performance metric

Include in this directory the job script(s) that you use to gather statistics for the performance metric on pace-ice. Additionally, describe what steps you've taken to ensure the quality of the statistics: how are you accounting for variability / noise? Does your benchmark show different performance on the first run than on subsequent runs?

If you are running your benchmark for multiple problem instances ($N$), include a plot of the performance metric for the different problem instances. (You can include error bars for maximum/minimum values of the performance metric for the same problem instance to convey variability.)

### 1.7 (3 pts): Compare your performance model to your statistics

If your performance model allowed you to make a concrete prediction of $V(N,P)$ before running the benchmark, compare (in a table or plot) the predictions and the actual measurements.

If your performance model includes coefficients that could not be estimated ahead of time, use measurements gathered during your experiments to get empirical values of those coefficients to fit the model to the data. Once you do this, answer the following question: is there a plausible explanation (in terms of the architecture of the machine and the nature of the algorithm) for why these coefficients have the value that they do?

Present additional timings and/or machine performance metrics (either for the full benchmark or key kernels) and make a case for why you think they best demonstrate what the biggest bottleneck to the performance of the benchmark is.
