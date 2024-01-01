/*
 *
 * Author: Shuojiang Liu
 * Class: ECE6122
 * Last Date Modified: November 6, 2023
 * Description: This program will implement a CUDA program to simulate a 2D random walk.
 * A random walk is a mathematical process that describes a path consisting of a sequence of random steps.
 * Simulate a large number of walkers taking steps either north, south, east, or west on a grid,
 * and calculate the average distance they travel from the origin.
 */

#include <cmath>
#include <iostream>

#include "cuda_runtime.h"
#include "curand_kernel.h"
#include "error.cuh"

#define BLOCK_SIZE 256

// GPU function to perform a random walk simulation
__global__ void randomWalkMethod(int *x, int *y, int numSteps, int numWalkers)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id >= numWalkers)
    {
        return;
    }

    curandState state;
    // we use thread index as the seed
    curand_init(threadIdx.x + blockIdx.x * blockDim.x, id, 0, &state);

    for (int step = 0; step < numSteps; step++)
    {
        float randomNumber = curand_uniform(&state);
        if (randomNumber < 0.25f)
            x[id] += 1;
        else if (randomNumber < 0.5f)
            x[id] -= 1;
        else if (randomNumber < 0.75f)
            y[id] += 1;
        else
            y[id] -= 1;
    }
}

// CPU function to calculate the average distance from the origin
float averageDistance(const int *x, const int *y, int numWalkers)
{
    float sumDistance = 0.0f;
    for (int i = 0; i < numWalkers; i++)
    {
        float distance = sqrtf(x[i] * x[i] + y[i] * y[i]);
        sumDistance += distance;
    }
    return sumDistance / static_cast<float>(numWalkers);
}

// Use Normal CUDA memory Allocation
void simulationNormal(int numWalkers, int numSteps)
{
    int* d_x, * d_y, * h_x, * h_y;
    cudaEvent_t start, stop;
    CHECK(cudaEventCreate(&start));
    CHECK(cudaEventCreate(&stop));

    CHECK(cudaMalloc(&d_x, numWalkers * sizeof(int)));
    CHECK(cudaMalloc(&d_y, numWalkers * sizeof(int)));
    h_x = new int[numWalkers];
    h_y = new int[numWalkers];

    CHECK(cudaEventRecord(start));

    int numBlocks = (numWalkers + BLOCK_SIZE - 1) / BLOCK_SIZE;
    randomWalkMethod<<<numBlocks, BLOCK_SIZE>>>(d_x, d_y, numSteps, numWalkers);

    CHECK(cudaDeviceSynchronize());

    CHECK(cudaEventRecord(stop));
    CHECK(cudaEventSynchronize(stop));

    CHECK(cudaMemcpy(h_x, d_x, numWalkers * sizeof(int), cudaMemcpyDeviceToHost));
    CHECK(cudaMemcpy(h_y, d_y, numWalkers * sizeof(int), cudaMemcpyDeviceToHost));

    float milliseconds = 0;
    CHECK(cudaEventElapsedTime(&milliseconds, start, stop));

    float averageDist = averageDistance(h_x, h_y, numWalkers);

    std::cout << "Normal CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << milliseconds * 1000 << std::endl;
    std::cout << "    Average distance from origin: " << averageDist << std::endl;

    CHECK(cudaFree(d_x));
    CHECK(cudaFree(d_y));

    CHECK(cudaEventDestroy(start));
    CHECK(cudaEventDestroy(stop));
}


// Use Pinned CUDA memory Allocation
void simulationPinned(int numWalkers, int numSteps)
{
    int *d_x, *d_y, *h_x, *h_y;
    cudaEvent_t start, stop;
    CHECK(cudaEventCreate(&start));
    CHECK(cudaEventCreate(&stop));

    CHECK(cudaMallocHost(&h_x, numWalkers * sizeof(int)));
    CHECK(cudaMallocHost(&h_y, numWalkers * sizeof(int)));

    CHECK(cudaMalloc(&d_x, numWalkers * sizeof(int)));
    CHECK(cudaMalloc(&d_y, numWalkers * sizeof(int)));

    CHECK(cudaEventRecord(start));

    int numBlocks = (numWalkers + BLOCK_SIZE - 1) / BLOCK_SIZE;
    randomWalkMethod<<<numBlocks, BLOCK_SIZE>>>(d_x, d_y, numSteps, numWalkers);

    CHECK(cudaDeviceSynchronize());

    CHECK(cudaEventRecord(stop));
    CHECK(cudaEventSynchronize(stop));

    CHECK(cudaMemcpy(h_x, d_x, numWalkers * sizeof(int), cudaMemcpyDeviceToHost));
    CHECK(cudaMemcpy(h_y, d_y, numWalkers * sizeof(int), cudaMemcpyDeviceToHost));

    float milliseconds = 0;
    CHECK(cudaEventElapsedTime(&milliseconds, start, stop));

    float averageDist = averageDistance(h_x, h_y, numWalkers);

    std::cout << "Pinned CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << milliseconds * 1000 << std::endl;
    std::cout << "    Average distance from origin: " << averageDist << std::endl;

    CHECK(cudaFree(d_x));
    CHECK(cudaFree(d_y));
    CHECK(cudaFreeHost(h_x));
    CHECK(cudaFreeHost(h_y));

    CHECK(cudaEventDestroy(start));
    CHECK(cudaEventDestroy(stop));
}

// Use Managed CUDA memory Allocation
void simulationManaged(int numWalkers, int numSteps)
{
    int *d_x, *d_y;
    cudaEvent_t start, stop;
    CHECK(cudaEventCreate(&start));
    CHECK(cudaEventCreate(&stop));

    CHECK(cudaMallocManaged(&d_x, numWalkers * sizeof(int)));
    CHECK(cudaMallocManaged(&d_y, numWalkers * sizeof(int)));

    CHECK(cudaEventRecord(start));

    int numBlocks = (numWalkers + BLOCK_SIZE - 1) / BLOCK_SIZE;
    randomWalkMethod<<<numBlocks, BLOCK_SIZE>>>(d_x, d_y, numSteps, numWalkers);

    CHECK(cudaDeviceSynchronize());

    CHECK(cudaEventRecord(stop));
    CHECK(cudaEventSynchronize(stop));

    float milliseconds = 0;
    CHECK(cudaEventElapsedTime(&milliseconds, start, stop));

    float averageDist = averageDistance(d_x, d_y, numWalkers);

    std::cout << "Managed CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << milliseconds * 1000 << std::endl;
    std::cout << "    Average distance from origin: " << averageDist << std::endl;

    CHECK(cudaFree(d_x));
    CHECK(cudaFree(d_y));

    CHECK(cudaEventDestroy(start));
    CHECK(cudaEventDestroy(stop));
}

/*
Takes as program input arguments the Number of Walkers,
and the number of steps each walker needs to take on a 2D integer grid.
Use command line flags to distinguish Number Walkers (-W) and (-I) for number of steps.
All the walkers start at the origin (0, 0).
*/
int main(int argc, char *argv[])
{
    int numWalkers = 1000;
    int numSteps = 10000;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-W") == 0)
            {
                numWalkers = atoi(argv[++i]);
            }
            else if (strcmp(argv[i], "-I") == 0)
            {
                numSteps = atoi(argv[++i]);
            }
        }
    }

    simulationNormal(numWalkers, numSteps);
    simulationPinned(numWalkers, numSteps);
    simulationManaged(numWalkers, numSteps);

    std::cout << "Bye" << std::endl;

    return 0;
}
