#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

const int iterations = 100;
const int NUMBER_OF_THREADS = 10000;

void threadFunction(int id, int iterations)
{
    this_thread::sleep_for(chrono::milliseconds(iterations));
}

void exec()
{
    // Record the start time
    high_resolution_clock::time_point start_time = high_resolution_clock::now();

    // Declare an array of thread objects
    thread threads[NUMBER_OF_THREADS];

    // Start the threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        threads[i] = thread(threadFunction, i, iterations);
    }

    // Join the threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        threads[i].join();
    }

    // Calculate the elapsed time in seconds
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast<duration<double>>(end_time - start_time);

    cout << scientific << elapsed_time.count();
}

int main()
{
    exec();

    return 0;
}
