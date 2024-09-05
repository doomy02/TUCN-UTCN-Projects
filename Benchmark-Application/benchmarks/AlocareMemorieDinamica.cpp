#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace chrono;

const int ARRAY_SIZE = 10000;
const int n = 100;

void exec()
{
    high_resolution_clock::time_point start_time = high_resolution_clock::now();

    for (long k = 0; k < n; k++)
    {
        int* dynamicArray = new int[ARRAY_SIZE];
        delete[] dynamicArray;
    }

    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast<duration<double>>(end_time - start_time);

    cout << scientific << elapsed_time.count();
}

int main()
{
    exec();

    return 0;
}
