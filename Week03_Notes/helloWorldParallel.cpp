#include <thread>
#include <iostream>
#include <array>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

void somethingToDo(int& num, const char* n)
{
	++num;
	n = "newChar";
	std::cout << "hello from a thread num=" << num << "name = "<<n<<std::endl;
}

void threadFunction()
{
	for (int i = 0; i < 10; ++i) 
	{
		auto id{ std::this_thread::get_id() };
		std::cout << "Hello from thread id = " << id << ", i= "<<i<< std::endl;
		std::this_thread::sleep_for(100ms);
	}
}
																			//tempArea: num_copy, name_copy
// A callable object
class MyClass 
{
	int callingCount{ 0 };
	std::mutex mx;
public:
	MyClass() {}
	void callme(int n) 
	{
		for (int i = 0; i < n; i++)
		{
			++callingCount;
			std::cout << "Thread 2 :: callable => function object\n";
		}
	}
	int count() const 
	{
		int ret{ callingCount };
		return ret;
	}
};

struct sharedBalance
{
	int balance;
	mutable std::mutex mx;

	void updateBalance(int val)
	{
		mx.lock();
		balance += val;
		mx.unlock();

	}

	int getBalance() const
	{
		mx.lock();
		int val{ balance };
		mx.unlock();
		return val;
	}
};

class Wallet
{
	int balance;
	int myDebt;
	std::mutex mutex;
	//sharedBalance bc;
public:
	Wallet() :balance(0) {}
	int getMoney() { return balance; }
	void deposit(int money)
	{
		mutex.lock(); // potentially blocking calls
		for (int i = 0; i < money; ++i)
		{
			balance++;
		}
		mutex.unlock();
		std::cout << "money = " << money << std::endl;
	}

	void withdraw(int amount)
	{

	}
};

int testMultithreadedWallet()
{
	Wallet walletObject;
	std::vector<std::thread> threads;
	for (int i = 0; i < 5; ++i) {
		threads.push_back(std::thread(&Wallet::deposit, &walletObject, 1000));
	}
	for (int i = 0; i < threads.size(); i++)
	{
		threads.at(i).join();
	}
	return walletObject.getMoney();
}

std::mutex g_mx;

int main()
{
//	int i{ 0 };
	const char* name{ "Reza" };
	int num1{ 4 };
	somethingToDo(num1, name);
	std::thread t(somethingToDo, std::ref(num1), name);
	t.join();  // this is a block call
	std::cout << name << std::endl;
	
	std::array<std::thread, 4> threads;
	for (int i = 0; i < threads.size(); ++i)
	{
		threads[i] = std::thread(threadFunction);
	}

	// here main thread do his work

	for (auto& th : threads)  // this is a synchronization point
		th.join();

	std::cout << "Main continue working....\n";

	
	int num{ 2 };
	// callable object:
	MyClass tho;

	// When calling methods in classes, you provide the method (Class::Method), then as first argument you give the thread the object reference
	// Finally, you pass the actual argument the class method needs to carry out is algorithm
	std::thread th2(&MyClass::callme, &tho, num);

	// do your stuff here
	//std::thread t2(incrementCount);
	// do more stuff..
	//t.join();
	//t2.join();
	th2.join();
	
	Wallet walletObject;
	std::thread tmon(&Wallet::deposit, &walletObject, 1000);

	tmon.join();


	int val = 0;
	for (int k = 0; k < 1000; k++)
	{
		if ((val = testMultithreadedWallet()) != 5000)
		{
			std::cout << "Error at count = " << k << "  Money in Wallet = " << val << std::endl;
			//break;
		}
	}
	return 0;
	// using lambda function for thread job:
	std::vector<std::thread> vecThs;
	for (int i = 0; i < 5; ++i) {
		vecThs.push_back(std::thread([]() {
			g_mx.lock();
			std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;

			g_mx.unlock();
			}));
	}

	for (auto& th : vecThs)
		th.join();

	return 0;
}

// exercise: Create an array of 4 threads and pass id= 0, 1, 2, 3 to each one. Each thread is printing "Hello from thread id= id".