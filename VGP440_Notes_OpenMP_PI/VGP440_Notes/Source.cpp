#include <iostream>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <vector>

using std::vector;

const long num_steps = 10000000;

void SerialVersion()
{
	/// <summary>
	/// Serial version of the integral calculation for the area of a curve
	/// </summary>
	/// <returns></returns>
	auto start = std::chrono::steady_clock::now();
	int repCount = 10;
	double pi = {};

	for (int repeat = 0; repeat < repCount; ++repeat)
	{
		int i;
		double sum = 0.0;

		double step = 1.0 / (double)num_steps;

		for (i = 0; i < num_steps; i++)
		{
			double x = (i + 0.5) * step;
			sum = sum + 4.0 / (1.0 + x * x);
		}

		pi = step * sum;
	}

	auto end = std::chrono::steady_clock::now();

	std::cout << "Pi for N = " << num_steps << " is " << pi << std::endl;

	std::cout << "Elapsed time in nanoseconds: "
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / repCount
		<< " ns" << std::endl;

	std::cout << "Elapsed time in microseconds: "
		<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / repCount
		<< " µs" << std::endl;

	std::cout << "Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / repCount
		<< " ms" << std::endl;

	std::cout << "Elapsed time in seconds: "
		<< std::chrono::duration_cast<std::chrono::seconds>(end - start).count() / repCount
		<< " sec";
}

void ParallelVersion()
{
	/// <summary>
	/// Serial version of the integral calculation for the area of a curve
	/// </summary>
	/// <returns></returns>
	auto start = std::chrono::steady_clock::now();
	int repCount = 1;
	double sum = 0.0;
	double pi = {};
	double step = 1.0 / static_cast<double>(num_steps);

	for (int repeat = 0; repeat < repCount; ++repeat)
	{
		int numThreads = omp_get_max_threads();

#pragma omp parallel
		{
			int i;
			int threadID = omp_get_thread_num();

			// Here add logic to divide the calculation in sections
			int start = static_cast<int>((threadID * num_steps) / numThreads);
			int end = static_cast<int>(((threadID + 1) * num_steps) / numThreads);

			if (threadID == (numThreads - 1))
			{
				end = static_cast<int>(num_steps);
			}

			double localSum{ 0 };
			for (i = start; i < end; i++)
			{
				double x = (i + 0.5) * step;
				localSum += 4.0 / (1.0 + x * x);
			}

			sum += localSum;
		}

		pi = sum * step;
	}

	auto end = std::chrono::steady_clock::now();

	std::cout << "Pi for N = " << num_steps << " is " << pi << std::endl;

	std::cout << "Elapsed time in nanoseconds: "
		<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / repCount
		<< " ns" << std::endl;

	std::cout << "Elapsed time in microseconds: "
		<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / repCount
		<< " µs" << std::endl;

	std::cout << "Elapsed time in milliseconds: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / repCount
		<< " ms" << std::endl;

	std::cout << "Elapsed time in seconds: "
		<< std::chrono::duration_cast<std::chrono::seconds>(end - start).count() / repCount
		<< " sec";
}

int main()
{
	//SerialVersion();
	std::cout.precision(15);
	ParallelVersion();

	return 0;
}