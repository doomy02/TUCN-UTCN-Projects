#include <iostream>
#include <stdio.h>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

const int ARRAY_SIZE = 10000;
const int n = 100;
// java nanotime
// c++ high resolution clock
void exec()
{
    duration<double> totalTime;

    // Run the program n times
    for (long k = 0; k < n; k++)
    {
        // Perform dynamic memory allocation
        int* dynamicArray = new int[ARRAY_SIZE];

        // Record the start time
        high_resolution_clock::time_point start_time = high_resolution_clock::now();
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            dynamicArray[i] = i + 1;
        }

        high_resolution_clock::time_point end_time = high_resolution_clock::now();
        duration<double> elapsed_time = duration_cast<duration<double>>(end_time - start_time);

        // Release the dynamically allocated memory
        delete[] dynamicArray;
        //cout << scientific << elapsed_time.count() << endl;
        totalTime += elapsed_time;
    }

    cout << scientific << totalTime.count();
}

int main()
{
    exec();


    return 0;
}
