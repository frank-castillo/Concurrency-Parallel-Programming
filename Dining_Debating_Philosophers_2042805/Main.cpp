#include "Philosopher.h"
#include <vector>

//This assignment is a classic example of use of concurrency. 5 philosophers sitting around a table with only 5 forks equally spaced around table.
// The philosophers take turn eating using the forks and while not eating, debating.
// Each philosopher can eat only when they holds both forks. Each eating takes 1 second per philosopher.
// We want to have them all eat for a total of 1 minute. The goal is to make it so all philosophers have equal chance in terms of time to eat and think.
// This means on average each philosopher eats around 12 turns spread along 60 min equally.
// You are going to use basic thread synchronization method mutex to serialize access to a fork.
// Create a class named Philosopher and add a method to Philosopher named eat() that simulated the act of eating at each turn.
// You are going to print a message saying something like
// "Philosopher 2 is eating using forks 2 and 3".
// Then it should pause for 4 seconds, before attempting to eat again. You can use the command sleep() for this purpose.
// IMPORTANT!You must use only c++ std::thread and std::mutex to implement this solution.No other c++ multi - threading constructs can be used.
// Also make sure you use lock_guard or unique_lock for mutex management.
// Zip you source files and .exe in release build in a folder for hand in.

int main()
{
    std::vector<std::mutex> forks = std::vector<std::mutex>(5); // Create 5 default mutex forks, one per philosopher
    std::vector<std::thread> philosophers; // Create the 5 philosophers
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    for (short i = 0; i < MAX_PHILOSOPHERS; ++i)
    {
        /// <summary>
        /// We instantiate a new thread in our vector
        /// On each new thread we initialize each philosopher with it's respective id and references to the two forks they can use
        /// We also let the thread know which member function they will carry out
        /// The object reference is the Philosopher we create
        /// </summary>
        /// <returns></returns>

        short rightForkIndex = (i + 1) % MAX_PHILOSOPHERS;
        philosophers.push_back(std::thread(&Philosopher::Eat, Philosopher(i, forks[i], forks[rightForkIndex])));
    }

    for (auto& philosopher : philosophers)
    {
        philosopher.join();
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto totalTime = end - start;

    std::cout << "Time it took for program to complete: " << std::chrono::duration_cast<std::chrono::seconds>(totalTime).count() << std::endl;
    std::cout << "Press any key to exit the program" << std::endl;

    std::getchar();

    return 0;
}