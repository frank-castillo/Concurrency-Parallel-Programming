#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

bool wordWasFound = false;

class BookReader
{
public:
    BookReader(std::string fileName, size_t threadId)
        : mFileName(fileName),
        mThreadId(threadId)
    {

    }

    ~BookReader() = default;

    void FindWordInFile(std::string& hiddenWord)
    {
        std::string word;
        std::ifstream file(mFileName); // Open the text file for reading

        if (file.is_open())
        {
            while (file >> word && !wordWasFound)
            {
                if (strcmp(word.c_str(), hiddenWord.c_str()) == 0)
                {
                    wordWasFound = true;
                    PrintOutput(hiddenWord);
                    exit(0);
                }
            }

            file.close();
        }
        else
        {
            std::cout << mFileName << " is not a valid book file. Please revise your argument!" << std::endl;
        }
    }

private:
    std::string mainOutputFile = "MainOutput.txt";
    std::string mFileName;
    size_t mThreadId;

    void StringConverter(std::string& str)
    {
        for (auto i = str.begin(); i < str.end(); ++i)
        {
            if (*i == ('Z' - 'A'))
            {
                *i + 32;
            }
            else if (*i != ('Z' - 'A') || *i != ('z' - 'a'))
            {

            }
        }
    }

    void PrintOutput(std::string& hiddenWord)
    {
        std::ofstream file(mainOutputFile); // Open the file for writing

        if (file.is_open())
        {
            file << "The word: " << hiddenWord << " was found in reading thread: " << mThreadId << " with book title: " << mFileName << "\n";
            file.close();

            std::cout << "Data written to file " << mainOutputFile << " was successful." << std::endl;
        }
        else
        {
            std::cout << "Unable to open the file." << std::endl;
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Not enough file arguments" << std::endl;
        return -1;
    }

    std::vector<std::thread> readerThreads;
    std::string wordToFind;

    int numberOfBooks = argc - 2;
    int threadCounter = std::thread::hardware_concurrency();

    if (numberOfBooks > threadCounter)
    {
        numberOfBooks = threadCounter;
        std::cout << "Number of books is bigger than what your system can handle! Only a part of them will be processed..." << std::endl;
    }

    wordToFind = argv[1];

    std::cout << "Creating Threads" << std::endl;
    for (size_t i = 0; i < numberOfBooks; ++i)
    {
        readerThreads.push_back(std::thread(&BookReader::FindWordInFile, BookReader(argv[i + 2], i), std::ref(wordToFind)));
    }

    std::cout << "Joining Threads" << std::endl;
    for (size_t i = 0; i < readerThreads.size(); ++i)
    {
        readerThreads[i].join();
    }

    return 0;
}