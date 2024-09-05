#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

const int ARRAY_SIZE = 10000;
const int n = 100;

void exec()
{
    // Record the start time
    high_resolution_clock::time_point start_time = high_resolution_clock::now();

    // Run the program NUMBER_OF_TESTS times
    for (long k = 0; k < n; k++)
    {
        // Perform static memory allocation
        int staticArray[ARRAY_SIZE];
    }

    // Record the end time
    high_resolution_clock::time_point end_time = high_resolution_clock::now();

    // Calculate the elapsed time in seconds
    duration<double> elapsed_time = duration_cast<duration<double>>(end_time - start_time);

    cout << scientific << elapsed_time.count();
}

int main()
{
    exec();

    return 0;
}
