# Report

## Major changes

### Parallel get pairs

![before_OptGetPairs](before_OptGetPairs.png)

From the profile above, we see that GetPairs loop takes surprisingly large amount of time. To optimize, it must be parallelized. Calculation in each box is independent, which can be paralleled naturally.

In original program, all interaction pairs are recorded on one list. To parallel the process of finding and recording interaction pairs, the potential conflict of several threads writing on the list at the same time should be diminished.

- Firstly, an array of pair lists was built in IXCreate. The number of lists in the array equals to the number of threads.

- Secondly, the finding process are executed on multiple threads (IXGetPairs). Every time one thread find an interacting pair, it push the pair onto the list it owns (IXPushpair).

- Finally, all the lists in the array are joined into one list at the end of IXGetPairs, which will be passed to accelerate.

![after_OptGetPairs](after_multivec.png)

From the profile after optimization, we see that:

1. `omp_fork` is called, which means that omp parallel routine is successfully called.

2. Percentage of time cost of `GetPairs` significantly reduce, which means that this work is useful.

### SIMD Remainder

A new function of computing the remainder of a number with regard to the dimension of the cell is written as MY_REMAINDER in the file "steric.h". The function is set under "omp declare simd", which let the code vecotrized better.

Idea comes from and proved in course lectures.

### Change the size of boxs

By changing the size of the box, we have control over roughly how many particles are in each box. Smaller boxes have fewer particles in them but have larger partitioning overhead. Since it doesn't affect the correctness of the program, here we only need to do some experiments and find a optimal size of the box.

Since the "radius" of particles is 1, minimum possible box length is 2, and maximum possible boxes along one axis is 10.

After the trial, we decided to set the boxdim as 8.

### Parallel the function of computing the forces

After finding the interacting pairs, the force between one pair is only related to the positions of the two particles, which is independent with other pairs. So the loop of computing forces should be paralleled with ease.

However, since there is the risk of adding forces exerted on one particle simultaneously, the code of adding forces onto the elements of the force array "U" are set atomic. Everytime only one thread can access to the position saving the force on a particle.

### Not using AoS

Both the structure of saving the positions of particles AoS and SA in vector.c were tried. Using SoA is quicker than AoS. The reason of such difference may be that SoA is better for vectorization.

## Note

To stay on track with other course materials, I didn't have that much time to spend on this late submission (at least for now). Thus this version is simply a roll back of our previous version, correcting a fatal error we made, in order to make up for the grade of project 1.
And thus this branch is only forked from one previous feature branch and added some minor change.

I am planning to implement some more "fancy" optimizations, but that depends on time we have. If you don't see any more push to this branch, please kindly consider this as my final submission.