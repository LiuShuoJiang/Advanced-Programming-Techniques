The file can be run on PACE ICE successfully.

> module load gcc mvapich2
> mpic++ ./MonteCarloSimulation.cpp -o Monto
> srun ./Monto -P 1 -N 100000