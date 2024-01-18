#include "External/AudioFile-master/AudioFile.h"

#include <complex>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <valarray>
#include <thread>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

void FFTHelperFunction(CArray& y)
{
    const size_t N = y.size();
    if (N <= 1) return;

    // divide
    CArray even = y[std::slice(0, N / 2, 2)];
    CArray  odd = y[std::slice(1, N / 2, 2)];

    FFTHelperFunction(even);
    FFTHelperFunction(odd);

    for (size_t k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        y[k] = even[k] + t;
        y[k + N / 2] = even[k] - t;
    }
}

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void FFTThreadedFunction(CArray& x, int num_threads = std::thread::hardware_concurrency())
{
    const size_t N = x.size();
    if (N <= 1) return;

    // Multi thread process
    // Divide into smaller subsets of valarray based on the available threads we have
    // We the use threads to compute the FFT of the smaller subsets
    // Join threads
    // Apply Cooley-Turkey Merge algorithm

    std::vector<CArray> subArrays(num_threads);

    for (int i = 0; i < num_threads; i++)
    {
        subArrays[i].resize(N / num_threads);

        for (int j = 0; j < N / num_threads; j++)
        {
            subArrays[i][j] = x[i * N / num_threads + j];
        }
    }

    std::vector<std::thread> fftThreads(num_threads);

    for (int i = 0; i < fftThreads.size(); ++i)
    {
        fftThreads[i] = std::thread(&FFTHelperFunction, std::ref(subArrays[i]));
    }

    for (auto& thread : fftThreads)
    {
        thread.join();
    }

    // Join the FFT's of each subArray
    for (int i = 0; i < fftThreads.size(); i++)
    {
        for (int j = 0; j < N / fftThreads.size(); j++)
        {
            x[i * N / fftThreads.size() + j] = subArrays[i][j];
        }
    }

    for (size_t k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * x[k + N / 2];
        x[k + N / 2] = x[k] - t;
        x[k] += t;
    }
}

void FFTSingleFunction(CArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    CArray even = x[std::slice(0, N / 2, 2)];
    CArray  odd = x[std::slice(1, N / 2, 2)];

    // conquer
    FFTSingleFunction(even);
    FFTSingleFunction(odd);

    // combine -> Threads
    for (size_t k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

int main(int argc, char* argv[])
{
    std::string fileName;

    // Validate the parameters for books we are passing
    if (argc < 2)
    {
        std::cout << "No audio file name was specified. Using default audio file." << std::endl;
        fileName = "wav_mono_16bit_44100.wav";
    }
    else
    {
        fileName = argv[1];
    }

    AudioFile<double> audioFile;

    // Load and obtain data from the audio file. Exit if file does not exist.
    if (!audioFile.load(fileName))
    {
        std::cout << "File does not exist! Please make sure you entered the right name and the audio file is in the source folder." << std::endl;
        return -1;
    }

    audioFile.printSummary();
    double numberOfSamples = audioFile.getNumSamplesPerChannel();

    std::vector<Complex> sampleData;
    sampleData.reserve(static_cast<size_t>(numberOfSamples));

    for (size_t i = 0; i < numberOfSamples; ++i)
    {
        sampleData.push_back(audioFile.samples[0][i]);
    }

    CArray singleThreadData(&sampleData[0], sampleData.size());
    CArray multiThreadData(&sampleData[0], sampleData.size());

    // Single Thread Method
    auto start = std::chrono::high_resolution_clock::now();

    FFTSingleFunction(singleThreadData);

    auto end = std::chrono::high_resolution_clock::now();
    auto totalTime = end - start;
    std::cout << "Time in milliseconds it took to process Single Thread FFT: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "\n";

    // Concurrent Method
    start = std::chrono::high_resolution_clock::now();

    FFTThreadedFunction(multiThreadData, std::thread::hardware_concurrency() - 2);

    end = std::chrono::high_resolution_clock::now();
    totalTime = end - start;
    std::cout << "Time in milliseconds it took to process Multi Thread FFT: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "\n";

    std::cout << "Printing result to a .txt file.\nThis will take a while!\nUse Python to run the code to visualize the data." << std::endl;

    // Compute the magnitude spectrum of the FFT output
    auto N = audioFile.getSampleRate();
    std::vector<double> mag_X(multiThreadData.size());

    for (int i = 0; i < multiThreadData.size(); i++) 
    {
        mag_X[i] = std::abs(multiThreadData[i]);
    }

    std::ofstream file("Magnitude_result.txt");

    for (int i = 0; i < mag_X.size(); i++)
    {
        if (i == mag_X.size() - 1)
        {
            file << mag_X[i];
            break;
        }

        file << mag_X[i] << std::endl;
    }

    return 0;
}