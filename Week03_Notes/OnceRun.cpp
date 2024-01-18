// 1- How to make a task be run only once by one of the involved threads.

#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>

using Ms = std::chrono::milliseconds;

std::once_flag oflag;
std::mutex mx;
std::mutex printMx;
int globalCount{};
std::ofstream *logfile;
void do_one_time_task() {
	//printMx.lock();
	//std::cout<<"I am in thread with thread id: "<< std::this_thread::get_id() << std::endl;
	//printMx.unlock();
	std::call_once(oflag, [&]() {
	//	std::cout << "Called once by thread: " << std::this_thread::get_id() << std::endl; 
	//	std::cout<<"Initialized globalCount to 3\n";
		globalCount = 3;
		logfile = new std::ofstream("testlog.txt");
		//logfile.open("testlog.txt");
		});
	std::this_thread::sleep_for(Ms(100));
	// some more here
	for(int i=0; i<10; ++i){	
		mx.lock();
		++globalCount;
		*logfile << "thread: "<< std::this_thread::get_id() <<", globalCount = " << globalCount << std::endl;
		std::this_thread::sleep_for(Ms(50));
		mx.unlock();
		std::this_thread::sleep_for(Ms(2));
	}

	std::this_thread::sleep_for(Ms(2000));
	std::call_once(oflag, [&]() {
		//std::cout << "Called once by thread: " << std::this_thread::get_id() << std::endl;
		//std::cout << "closing logfile:\n";
		logfile->close();
		});

}

int main()
{
	std::array<std::thread, 5> thArray;
	for (int i = 0; i < 5; ++i)
		thArray[i] = std::thread(do_one_time_task);

	for (int i = 0; i < 5; ++i)
		thArray[i].detach();

	//std::this_thread::sleep_for(Ms(5000));
	std::cout << "Main: globalCount = " << globalCount << std::endl;

	return 0;
}

