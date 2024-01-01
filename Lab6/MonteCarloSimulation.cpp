/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 4th, 2023
Description:
Integral simulation using Monte-Carlo method.
*/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>

#include <mpi.h>

// Function to estimate integral of x^2 from 0 to 1
double calculateIntegral1(long N)
{
	double sum = 0.0;
	for (long i = 0; i < N; i++)
	{
		double x = static_cast<double>(rand()) / RAND_MAX;
		sum += x * x;
	}
	return sum / N;
}

// Function to estimate integral of e^(-x^2) from 0 to 1
double calculateIntegral2(long N)
{
	double sum = 0.0;
	for (long i = 0; i < N; i++)
	{
		double x = static_cast<double>(rand()) / RAND_MAX;
		sum += exp(-x * x);
	}
	return sum / N;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Only rank 0 handles the command-line arguments
	int P;
	long N;
	if (rank == 0)
	{
		if (argc != 5)
		{
			std::cerr << "Usage: " << argv[0] << " -P [1|2] -N <number_of_samples>" << std::endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		// Parsing command line arguments
		P = std::stoi(argv[2]);
		N = std::stol(argv[4]) / world_size;
	}

	// Broadcast the integral choice and number of samples to all processors
	MPI_Bcast(&P, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&N, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	// Seed the random number generator
	srand(static_cast<unsigned int>(time(nullptr)) + rank);

	double result = 0.0;
	switch (P)
	{
	case 1:
		result = calculateIntegral1(N);
		break;
	case 2:
		result = calculateIntegral2(N);
		break;
	default:
		if (rank == 0)
			std::cerr << "Invalid choice of integral. Please choose 1 or 2." << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	double globalResult = 0.0;
	MPI_Reduce(&result, &globalResult, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		globalResult /= world_size;
		std::cout << "The estimate for integral " << P << " is " << globalResult << std::endl;
		std::cout << "Bye!" << std::endl;
	}

	MPI_Finalize();
	return 0;
}