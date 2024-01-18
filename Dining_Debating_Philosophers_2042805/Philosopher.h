#pragma once
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

constexpr int MAX_PHILOSOPHERS = 5;

class Philosopher
{
public:
    Philosopher(int threadId, std::mutex& leftFork, std::mutex& rightFork)
        : mId(threadId), mLeftFork(leftFork), mRightFork(rightFork)
    {
    }

    void Eat()
    {
        for (short turn = 0; turn < 12; ++turn)
        {
            /// <summary>
            /// We are using defer lock to not lock the fork as soon as we instantiate a philosopher
            /// After we defer the lock, we then lock the new unique_locks in order to avoid a deadlock
            /// Afterwards we print the required message and sleep the thread for an specific time amount
            /// 
            /// Code references:
            /// https://en.cppreference.com/w/cpp/thread/unique_lock
            /// https://en.cppreference.com/w/cpp/thread/lock_tag
            /// https://stackoverflow.com/a/10613664
            /// </summary>

            std::this_thread::sleep_for(std::chrono::seconds(4));

            std::unique_lock<std::mutex> leftLock(mLeftFork, std::defer_lock);
            std::unique_lock<std::mutex> rightLock(mRightFork, std::defer_lock);
            std::lock(leftLock, rightLock);

            eatingMessage << "Philosopher number = " << mId << " is currently eating using forks = " << mId << " and " << (mId + 1) % MAX_PHILOSOPHERS;
            std::cout << eatingMessage.str() << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));

            eatingMessage << "Philosopher number = " << mId << " finished eating";
            std::cout << eatingMessage.str() << std::endl;
        }
    }

private:
    int mId = INT_MIN;
    std::stringstream eatingMessage;
    std::mutex& mLeftFork;
    std::mutex& mRightFork;
};