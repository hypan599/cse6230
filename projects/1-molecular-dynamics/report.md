# Report

## Major changes

### Parallel get pairs

### SIMD Remainder

## Note

git-branching: since we're simply rolling back, I'll continue work in this branch without opening up a new branch.



### Ori

Change the size of boxs into an optimized one:

If we only consider the time consumed by the step of finding interaction pairs, the best size should be the SMALLEST box size which fulfills the requirement that particles inside one box only interact with particles in the same box and in the nearby boxes.

Since the "radius" of the kind of particles is 1, based on the geometrical relationship, the optimized size of boxes should be 2×2×2

. So we changed the coefficient boxdim as

202=10

However, if the number of boxes is larger, there will also be extra time spent on the box-creating process. Since there is a larger array for labeling and saving boxes, the time spent on accessing the particles inside a box will also be longer.

After the trial, we decided to set the boxdim as 8
.

    Parallel the loop of finding interaction pairs:

    We looked at profile of the program, and the GetPairs loop takes surprisingly large amount of time. To optimize, it must be parallelized. Calculation in each box is independent, which can be paralleled naturally. The most difficult part here is the data structure.

    In original program, all interaction pairs are recorded on one list. To parallel the process of finding and recording interaction pairs, the potential conflict of several threads writing on the list at the same time should be diminished.

    Firstly, an array of pair lists was built in IXCreate. The number of lists in the array is the number of threads.

    Secondly, the finding process are executed on multiple threads (IXGetPairs). Every time one thread find an interacting pair, it push the pair onto the list it owns (IXPushpair).

    Finally, all the lists in the array are hoined into one list at the end of IXGetPairs, which will be sent to accelerate.

    Change the frequency of updating interaction pairs:

    In the original sequential program, the linked list of interaction pairs is updated in every time step. However, if the average speed of particles is not as high as flying over the distance as the diameter of the particle in a time step, it is not necessary to update it so frequently: the interaction pairs can keep interact in more than one time step, the two particles without interaction are also not so easy to correlate in a time step.

    The structure of verlet_step and accelerate were changed to make them more flexible on modifying the period or frequency of updating interaction pairs.

    We increase the interval to 6

    time steps and the diffusion constant calculated is still pretty close to the original, so we use this number.

    Parallel the function of computing the forces:

    After finding the interacting pairs, the force between one pair is only related to the positions of the two particles, which is independent with other pairs. So the loop of computing forces are paralleled.

    However, since there is the risk of adding forces exerted on one particle simultaneously, the code of adding forces onto the elements of the force array "U" are set atomic. Everytime only one thread can access to the position saving the force on a particle.

    Replace the function REMAINDER by using omp simd:

    A new function of computing the remainder of a number with regard to the dimension of the cell is written as MY_REMAINDER in the file "steric.h". The function is set under "omp declare simd", which let the code vecotrized better.

    Not using AoS:

    Both the structure of saving the positions of particles AoS and SA in vector.c were tried. Using SoA is quicker than AoS. The reason of such difference may be that SoA is better for vectorization.

    Play with parallel schedule:

    I have tried different parallel schedule, in some case runtime gives better performance than static, and static, 16 is also sometimes better than just static.

