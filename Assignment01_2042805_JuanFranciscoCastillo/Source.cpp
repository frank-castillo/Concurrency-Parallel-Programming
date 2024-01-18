#include <iostream>
#include <chrono>
#include <cstdio>
#include <math.h>
#include <limits.h>
#include <omp.h>
#include <random>

using namespace std::chrono;
using std::cout;
using std::endl;

/**
* f(x) = 4.0/(1 + x^2)
* numerical integration of f(x) over range of 0 to 1 which should be equal to PI
**/
const long long num_steps{ 1000000000 };
const double M_PI{ 3.141592653589793 };

void PrintResults(double timeDifference, double pi, long num_steps)
{
	cout << "Pi for " << num_steps << " number of steps is: " << pi << endl;
	cout << "Total time taken for computation in Milliseconds: " << timeDifference << endl;
}

double PI_Integral()
{
	auto start = std::chrono::steady_clock::now();
	const double step = 1.0 / static_cast<double>(num_steps);
	double sum = { 0.0 }; // Adding to this double to avoid bottleneck
	double pi = { 0.0 };

#pragma omp parallel num_threads(8)
	{
		int numThreads = omp_get_num_threads();
		int threadID = omp_get_thread_num();

		// Here add logic to divide the calculation in sections
		int start = threadID * static_cast<int>(num_steps / numThreads);
		int end = (threadID + 1) * static_cast<int>(num_steps / numThreads);

		if (threadID == (numThreads - 1))
		{
			end = static_cast<int>(num_steps);
		}

		double localSum{ 0 };
		for (int i = start; i < end; i++)
		{
			double x = (i + 0.5) * step;
			localSum += 4.0 / (1.0 + x * x);
		}

		sum += localSum;
	}
#pragma omp critical

	pi = sum * step;

	auto end = std::chrono::steady_clock::now();
	double timeDifference = static_cast<double>(duration_cast<milliseconds>(end - start).count());

	cout << "Integral Calculation of Pi: " << endl;
	PrintResults(timeDifference, pi, num_steps);

	return pi;
}

double PI_MonteCarlo()
{
	// Create random number generator
	std::default_random_engine generator;

	// Set range
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);

	auto start = std::chrono::steady_clock::now();
	long pointsInsideCircle{ 0 };
	double pi{ 0 };

	// Parallelization with reduction technique
	// Everything is reduced simultaneously
#pragma omp parallel for reduction(+: pointsInsideCircle)
	for (long i = 0; i < num_steps; i++)
	{
		double x, y = { 0.0 };

		// Obtain the random number for values of X and Y relative to the 2D plane we create to count the points inside the circle
		x = distribution(generator);
		y = distribution(generator);

		// Check if numbers are inside the unot circle
		if (x * x + y * y <= 1.0)
		{
			++pointsInsideCircle;
		}
	}
#pragma omp critical

	//Calcuting the ratio and as a result the pi
	// We multiply by four as the multiplier of the area of the square
	// number of points inside circle / number of all points * area of square
	pi = (static_cast<double>(pointsInsideCircle) / static_cast<double>(num_steps) * 4.0);

	auto end = std::chrono::steady_clock::now();
	double timeDifference = static_cast<double>(duration_cast<milliseconds>(end - start).count());

	cout << "MonteCarlo Calculation of Pi: " << endl;
	PrintResults(timeDifference, pi, num_steps);

	return pi;
}

int main()
{
	cout.fixed;
	cout.precision(18);
	 
	cout << "Integral PI difference from actual PI value " << abs(PI_Integral() - M_PI) << endl;
	cout << "MonteCarlo PI difference from actual PI value " << abs(PI_MonteCarlo() - M_PI) << endl;

	std::getchar();

	return 0;
}