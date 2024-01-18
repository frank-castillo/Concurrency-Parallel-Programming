#include <mutex>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

struct SharedInt {

    std::timed_mutex m;
    int i;
    SharedInt(int i=0) : i(i) {}

    void mul(int x) 
    {
      //  auto curr{ std::chrono::system_clock::now() };

        // It will try to lock for that specific amount of time
        // If you manage to get inside the function, carry out the operation
        // If the time passes and you are still waiting to access that memory, stop!
        // Then carry on with the other operations and code calls from the else bracket
        if (m.try_lock_for(10ms))  // timed blocking
        {
            i *= x;
        }
        else
             std::cout << "m is already in use!\n";
    
       // static  int count{ 0 };
       // ++count;
    }

    void div(int x) 
    {
      //  if (m.try_lock_for(Ms(100)))
        std::lock_guard<std::timed_mutex> lk(m);
        i /= x;
    }


    void mult_div(int a, int b)
    {
     //   m.lock();
        mul(a);
        div(b);
       // m.unlock();
    }

};

int main()
{
    SharedInt m{ 25 };
    std::thread multiplicationThd([&m]() {for (int i = 1; i < 5; ++i) m.mul(i); });
    std::thread divisionThd([&m]() {for (int i = 1; i < 5; ++i) m.div(i); });
    // add a new thread, named mult_divThd which both multiplyes m by 2i and divides by i
  //  std::thread multDivThd([&m]() {for (int i = 1; i < 5; ++i) m.mult_div(2*i, i); });

    std::cout << "m=" << m.i << std::endl;

    multiplicationThd.join();
    divisionThd.join();
   // multDivThd.join();

    return 0;
}