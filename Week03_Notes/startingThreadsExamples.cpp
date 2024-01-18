//Starting threads:

#include <thread>
#include <iostream>

//void hello(){
//    std::cout << "Hello from thread " << std::endl;
//}
//
//int main(){
//    std::jthread t1(hello);
//    t1.join();
//
//    return 0;
//}

/////////////////////////////////////////////////////////////////////
//Distinguishing threads
#include <vector>
#include <mutex>

//void hello(){
//    std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
//}

//int main(){
//    std::vector<std::thread> threads;
//
//    for(int i = 0; i < 5; ++i){
//        threads.push_back(std::thread(hello));
//    }
//
//    for(auto& thread : threads){
//        thread.join();
//    }
//
//    return 0;
//}

////////////////////////////////////////////////////////
//Start a thread with a lambda
//int main(){
//    std::vector<std::thread> threads;
//
//    for(int i = 0; i < 5; ++i){
//        threads.push_back(std::thread([](){
//            std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
//        }));
//    }
//
//    for(auto& thread : threads){
//        thread.join();
//    }
//
//    return 0;
//}


//////////////////////////////////////////////
//Synchronization issues

//struct Counter {
//    int value;
//
//    Counter() : value(0){}
//
//    void increment(){
//        ++value; // There is a race condition on this call if accessed by multiple threads
//    }
//};

//int main(){
//    Counter counter;
//
//    std::vector<std::thread> threads;
//    for(int i = 0; i < 5; ++i){
//        threads.push_back(std::thread([&counter](){
//            for(int i = 0; i < 100; ++i){
//                counter.increment();
//            }
//        }));
//    }
//
//    for(auto& thread : threads){
//        thread.join();
//    }
//
//    std::cout << counter.value << std::endl;
//
//    return 0;
//}

// using mutex to make our Counter thread-safe
//struct Counter {
//    std::mutex mutex;
//    int value;
//
//    Counter() : value(0) {}
//
//    void increment(){
//        mutex.lock();
//        ++value;
//        mutex.unlock();
//    }
//
//    void decrement() {
//        mutex.lock();
//        --value;
//        mutex.unlock();
//    }
//};

// Not using mutex as the other struct already manages the lock 
struct Counter {
	int value;

	Counter() : value(0) {}

	void increment()
	{
		++value;
	}

	void decrement()
	{
		--value;
	}
};

// Automatic management of locks
struct ConcurrentSafeCounter {
	std::mutex mutex;
	Counter counter;

	void increment() {
		//std::lock_guard<std::mutex> guard(mutex);
		std::unique_lock<std::mutex> guard(mutex);
		counter.increment();
		guard.unlock(); // Here I can do other stuff that is not atomic and then leave it open or close it again!
	}

	void decrement() {
		std::lock_guard<std::mutex> guard(mutex);
		counter.decrement();
	}
};

int main() {
	ConcurrentSafeCounter counter;

	std::vector<std::thread> threads;
	for (int i = 0; i < 5; ++i) {
		threads.push_back(std::thread([&counter]() {
			for (int i = 0; i < 100; ++i) {
				counter.increment();
			}
			}));
	}

	for (auto& thread : threads) {
		thread.join();
	}

	std::cout << counter.counter.value << std::endl;

	return 0;
}
//////////////////////////////////////////////////////////