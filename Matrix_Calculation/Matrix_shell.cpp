#include <memory>
#include <random>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <thread>
#include <assert.h>

struct Matrix
{
	int nRows, nCols;
	long long** data;

	Matrix() = default;

	Matrix(int r, int c) : nRows{ r }, nCols{ c }
	{
		data = new long long* [nRows];
		for (int i = 0; i < nRows; ++i)
		{
			data[i] = new long long[nCols];
		}
	}

	~Matrix()
	{
		if (nRows == 0)
			return;
		for (int i = 0; i < nRows; ++i)
			delete[] data[i];
		delete[] data;
	}

	// initialize the matrix by random numbers
	void init()
	{
		std::srand(std::time(0));
		for (int i = 0; i < nRows; ++i)
			for (int j = 0; j < nCols; ++j)
				data[i][j] = std::rand() % 20 - 10;
	}

	void CreatePrintMatrix(Matrix& a, int r, int c)
	{
		a.nRows = r;
		a.nCols = c;

		a.data = new long long* [a.nRows];

		for (int i = 0; i < nRows; ++i)
		{
			a.data[i] = new long long[a.nCols];
		}

		a.init();
		a.print();
	}

	void print(int nRows, int nCols)
	{
		std::cout << "Matrix:\n";
		for (int i = 0; i < nRows; ++i)
		{
			for (int j = 0; j < nCols; ++j)
				std::cout << data[i][j] << ", ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	void print()
	{
		std::cout << "Matrix:\n";

		for (int i = 0; i < nRows; ++i)
		{
			for (int j = 0; j < nCols; ++j)
				std::cout << data[i][j] << ", ";
			std::cout << std::endl;
		}

		std::cout << std::endl;
	}

	static void Mult(Matrix const& A, Matrix const& B, Matrix* result)
	{
		//checking if A's num of Col is equal to B's num of Row
		//assert(A.nCols == B.nRows && "Matrices num of Cols and Rows do not match!");
		if (A.nCols != B.nRows)
		{
			std::cout << "A and B dimensions do not match!" <<
				" A.nCols=" << A.nCols << ", B.nRows=" << B.nRows << std::endl;
			return;
		}

		// do the multiplication
		for (int i = 0; i < A.nRows; ++i)
		{
			for (int j = 0; j < B.nCols; ++j)
			{
				long long res = 0;
				for (int k = 0; k < A.nCols; ++k)
					res += A.data[i][k] * B.data[k][j];
				result->data[i][j] = res;
			}
		}
	}

	static void MultThread(Matrix const& A, Matrix const& B, Matrix* result, int threadID, int maxThreads)
	{
		int start = (threadID * A.nRows) / maxThreads;
		int end = ((threadID + 1) * A.nRows) / maxThreads;

		if (threadID == maxThreads - 1)
		{
			end = A.nRows;
		}

		// do the multiplication
		for (int i = start; i < end; ++i)
		{
			for (int j = 0; j < B.nCols; ++j)
			{
				long long res = 0;

				for (int k = 0; k < A.nCols; ++k)
				{
					res += A.data[i][k] * B.data[k][j];
				}

				result->data[i][j] = res;
			}
		}
	}

	static void ThreadedMultiplication(Matrix const& A, Matrix const& B, Matrix* result)
	{
		// use maxNumHThreads -1 to parallelize the multiplication.
		auto maxNumHThreads = std::thread::hardware_concurrency() - 1;
		std::vector<std::thread> multiplicationThreadVector;

		for (int threadCounter = 0; threadCounter < maxNumHThreads; ++threadCounter)
		{
			multiplicationThreadVector.push_back(std::thread(MultThread, std::ref(A), std::ref(B), result, threadCounter, maxNumHThreads));
		}

		for (auto& thread : multiplicationThreadVector)
		{
			thread.join();
		}

		std::cout << "Number of threads: " << maxNumHThreads << std::endl;
	}
};

void SingleThreadMatrixCalculation(Matrix& a, Matrix& b)
{
	auto start = std::chrono::steady_clock::now();

	Matrix* result = new Matrix(a.nRows, b.nCols);
	Matrix::Mult(a, b, result);

	auto end = std::chrono::steady_clock::now();
	double timeDifference = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
	double timeDifferenceMili = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	result->print(10, 10);

	std::cout << "Serial Calculation of matrix: " << std::endl;
	std::cout << "Total time taken for computation in NanoSeconds: " << timeDifference << std::endl;
	std::cout << "Total time taken for computation in MiliSeconds: " << timeDifferenceMili << std::endl;
}

void MultiThreadMatrixCalculation(Matrix& a, Matrix& b)
{
	auto start = std::chrono::steady_clock::now();

	Matrix* result = new Matrix(a.nRows, b.nCols);

	//checking if A's num of Col is equal to B's num of Row
	if (a.nCols != b.nRows)
	{
		std::cout << "A and B dimensions do not match!" <<
			" A.nCols=" << a.nCols << ", B.nRows=" << b.nRows << std::endl;
		return;
	}

	Matrix::ThreadedMultiplication(a, b, result);

    auto end = std::chrono::steady_clock::now();
    double timeDifference = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    double timeDifferenceMili = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    result->print(10, 10);

    std::cout << "Serial Calculation of matrix: " << std::endl;
    std::cout << "Total time taken for computation in NanoSeconds: " << timeDifference << std::endl;
    std::cout << "Total time taken for computation in MiliSeconds: " << timeDifferenceMili << std::endl;
}

int main()
{
	//time this for single threaded and multi-threaded and compare the results.

    int amount = 5000;

    Matrix a(30, amount);
    a.init();
    //a.print();

    Matrix b(amount, 60);
    b.init();
    //b.print();

	// Single thread
	SingleThreadMatrixCalculation(a,b);

	// Multi Thread
	MultiThreadMatrixCalculation(a,b);

	return 0;
}