#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

// Consumer producer case pattern
// When thread is empty, no pulling
// When thread full, no pushing

using namespace std;

struct BoundedBuffer
{
    int* buffer{};
    int capacity{};

    int frontIndex{};
    int rearIndex{};
    int count{};

    mutex m;
    condition_variable not_full_cv;
    condition_variable not_empty_cv;

    BoundedBuffer(int cap) : capacity(cap), frontIndex(0), rearIndex(0), count(0)
    {
        buffer = new int[capacity];
    }

    ~BoundedBuffer()
    {
        delete[] buffer;
    }

    void Deposit(int data)
    {
        unique_lock<mutex> lock(m);
        not_full_cv.wait(lock, [this] {return count != capacity; });
        buffer[rearIndex] = data;
        rearIndex = (rearIndex + 1) % capacity;
        ++count;
        lock.unlock();
        not_empty_cv.notify_one();
    }

    int Fetch()
    {
        unique_lock<mutex> lock(m);
        not_empty_cv.wait(lock, [this] {return count != 0; });
        int result = buffer[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        --count;
        lock.unlock();
        not_full_cv.notify_one();
        return result;
    }
};

void Consume(int id, BoundedBuffer& buffer)
{
    for (short i = 0; i < 50; ++i)
    {
        int val = buffer.Fetch();
        cout << "Consumer " << id << " fetched " << val << endl;
        std::this_thread::sleep_for(chrono::milliseconds(150));
    }
}

void Produce(int id, BoundedBuffer& buffer)
{
    for (short i = 0; i < 75; ++i)
    {
        buffer.Deposit(i + 1);
        cout << "Consumer " << id << " deposited " << (i + 1) << endl;
        std::this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main()
{
    BoundedBuffer buf(100);
    thread c1(Consume, 0, ref(buf));
    thread c2(Consume, 1, ref(buf));
    thread c3(Consume, 2, ref(buf));

    thread p1(Produce, 0, ref(buf));
    thread p2(Produce, 1, ref(buf));

    cout << "Waiting for the producer-consumers to finish";

    c1.join();
    c2.join();
    c3.join();
    p1.join();
    p2.join();

    cout << "All Finished" << endl;

    return 0;
}