#include <iostream>
#include <vector>
#include <thread>
#include <sstream>

void HelloWorld()
{
	// the thread's Id here is given automatically by the system
	// They are resources given to you by the system
	//std::cout << "Hello World from thread: " << std::this_thread::get_id() << std::endl;
	std::stringstream ss;

	auto threadId = std::this_thread::get_id();
	ss << threadId;
	std::string printMessage = "Hello World from thread: " + ss.str() + "\n";
	std::cout << printMessage;
}

int main()
{
	std::vector<std::thread> threads;

	// Synchronus way to create threads
	for (int i = 0; i < 5; ++i)
	{
		threads.emplace_back(std::thread(HelloWorld));
	}

	std::cout << "In main thread, about to exit" << std::endl;

	for (auto& thread : threads)
	{
		thread.join();
	}

	std::cout << "Done joining all the children threads" << std::endl;

	// The cout will be accessed by all the threads at the same time as it is a single buffer, so the message will be a mess as all threads
	// are trying to access the exact same buffer at the sam time
	// Use a string to print it all out at the same time
}