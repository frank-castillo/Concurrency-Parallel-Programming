#include <chrono>
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

using WordCountMapType = std::unordered_map<std::string, size_t>;
constexpr unsigned short MAX_QUEUE_SIZE = 1000;

std::unordered_map<std::string, size_t> main_UnorderedMap;
std::mutex main_MapMutex;
std::condition_variable queueIsFull;

class BookReader
{
public:
    BookReader() = default;

    ~BookReader() = default;

    void FlushQueue()
    {
        //std::cout << "I am waiting to empty queue for book: " << mFileName << std::endl;
        std::unique_lock<std::mutex> mapLock(main_MapMutex);
        queueIsFull.wait(mapLock, [this]() {return !thread_WordsQueue.empty(); });

        //std::cout << "I am dropping my load for book: " << mFileName << std::endl;
        while (!thread_WordsQueue.empty())
        {
            std::pair<std::string, size_t> word = thread_WordsQueue.front();
            ++main_UnorderedMap[word.first] += word.second;
            thread_WordsQueue.pop();
        }

        mapLock.unlock();
        queueIsFull.notify_all();
    }

    void ReadBookFile(std::string fileName)
    {
        start = std::chrono::high_resolution_clock::now();
        mFileName = fileName;
        std::string word;
        std::ifstream file(fileName); // Open the text file for reading

        if (file.is_open())
        {
            while (file >> word)
            {
                if (word.size() > 1)
                {
                    ++thread_WordsMap[word];
                }
            }

            file.close();
            PopulateQueue();
        }
        else
        {
            std::cout << mFileName << " is not a valid book file. Please revise your argument!" << std::endl;
        }
    }

    void PopulateQueue()
    {
        for (auto& word : thread_WordsMap)
        {
            thread_WordsQueue.push(std::make_pair(word.first, word.second));

            if (thread_WordsQueue.size() >= MAX_QUEUE_SIZE)
            {
                FlushQueue();
            }
        }

        FlushQueue(); // Guarantee to get all words from the document
        end = std::chrono::high_resolution_clock::now();
    }

    void PrintBookInformation()
    {
        std::vector<std::pair<std::string, std::size_t>> topWordsCount;
        std::string outputFileString = mFileName.substr(0, mFileName.rfind("."));
        outputFileString += "_output.txt";

        for (auto iter = thread_WordsMap.begin(); iter != thread_WordsMap.end(); ++iter)
        {
            topWordsCount.push_back(*iter);
        }

        std::sort(topWordsCount.begin(), topWordsCount.end(), [](const auto& a, const auto& b)
            {
                return a.second > b.second;
            });

        std::ofstream file(outputFileString); // Open the file for writing

        if (file.is_open())
        {
            file << "Analyzing: " << mFileName << "\n";
            std::cout << "Analyzing: " << mFileName << std::endl;

            for (int i = 0; i < 20 && i < topWordsCount.size(); ++i)
            {
                // Add book local info to a single string in order to later write out to the .text file
                file << topWordsCount[i].first << ": " << topWordsCount[i].second << " occurrences." << "\n";
                std::cout << topWordsCount[i].first << ": " << topWordsCount[i].second << " occurrences." << std::endl;
            }

            auto totalTime = end - start;
            file << "Time in milliseconds it took to process the book: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "\n";
            std::cout << "Time in milliseconds it took to process the book: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << std::endl;
            std::cout << std::endl;

            file.close();
        }
        else
        {
            std::cout << "Unable to open the file." << std::endl;
        }
    }

private:
    WordCountMapType thread_WordsMap;
    std::queue<std::pair<std::string, size_t>> thread_WordsQueue;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    std::string thread_BookInfo;
    std::string mFileName;
};

int main(int argc, char* argv[])
{
    // Validate the parameters for books we are passing
    if (argc < 2)
    {
        std::cout << "Not enough book arguments" << std::endl;
        return -1;
    }

    main_UnorderedMap.clear();

    std::vector<std::pair<std::string, std::size_t>> mainTopWordsCount;
    std::vector<BookReader*> bookReaders;
    std::vector<std::thread> readerThreads;
    std::vector<std::string> booksNames;
    std::string mainOutputFile = "MainOutput.txt";

    int numberOfBooks = argc - 1;
    int threadCounter = std::thread::hardware_concurrency();

    if (numberOfBooks > threadCounter)
    {
        numberOfBooks = threadCounter;
        std::cout << "Number of books is bigger than what your system can handle! Only a part of them will be processed..." << std::endl;
    }

    for (size_t i = 0; i < numberOfBooks; ++i)
    {
        booksNames.push_back(argv[i + 1]);
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    std::cout << "Creating Readers" << std::endl;
    for (size_t i = 0; i < numberOfBooks; ++i)
    {
        bookReaders.push_back(new BookReader());
    }

    std::cout << "Creating Threads" << std::endl;
    for (size_t i = 0; i < bookReaders.size(); ++i)
    {
        readerThreads.push_back(std::thread(&BookReader::ReadBookFile, bookReaders[i], booksNames[i]));
    }

    std::cout << "Joining Threads" << std::endl;
    for (size_t i = 0; i < readerThreads.size(); ++i)
    {
        readerThreads[i].join();
    }

    std::cout << "Printing Book Information" << std::endl;
    std::cout << std::endl;

    for (size_t i = 0; i < readerThreads.size(); ++i)
    {
        bookReaders[i]->PrintBookInformation();
    }

    std::cout << "Clearing Memory" << std::endl;
    std::cout << std::endl;

    for (size_t i = 0; i < readerThreads.size(); ++i)
    {
        delete bookReaders[i];
    }

    bookReaders.clear();

    for (auto iter = main_UnorderedMap.begin(); iter != main_UnorderedMap.end(); ++iter)
    {
        mainTopWordsCount.push_back(*iter);
    }

    std::sort(mainTopWordsCount.begin(), mainTopWordsCount.end(), [](const auto& a, const auto& b)
        {
            return a.second > b.second;
        });

    std::ofstream file(mainOutputFile); // Open the file for writing

    if (file.is_open())
    {
        file << "Printing out the most used words in all books: " << "\n";
        std::cout << "Printing out the most used words in all books: " << std::endl;

        for (int i = 0; i < 20 && i < mainTopWordsCount.size(); ++i)
        {
            // Add book local info to a single string in order to later write out to the .text file
            file << mainTopWordsCount[i].first << ": " << mainTopWordsCount[i].second << " occurrences." << "\n";
            std::cout << mainTopWordsCount[i].first << ": " << mainTopWordsCount[i].second << " occurrences." << std::endl;
        }

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto totalTime = end - start;

        file << "Time in milliseconds for program to complete: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "\n";

        file.close();
        std::cout << "Data written to file " << mainOutputFile << " was successful." << std::endl;
    }
    else
    {
        std::cout << "Unable to open the file." << std::endl;
    }

    return 0;
}