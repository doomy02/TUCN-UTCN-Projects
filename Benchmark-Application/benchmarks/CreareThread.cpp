#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include <numeric>

using namespace std;
using namespace chrono;

const int NUMBER_THREADS = 10000;

void threadFunction(int& result) {
}

void exec() {
    // Record the start time
    high_resolution_clock::time_point start_time = high_resolution_clock::now();

    vector<thread> threads;
    vector<int> results(NUMBER_THREADS, 0);

    for (int i = 0; i < NUMBER_THREADS; ++i) {
        threads.emplace_back(threadFunction, ref(results[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Calculate the elapsed time in seconds
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast<duration<double>>(end_time - start_time);

    cout << scientific << elapsed_time.count();
}

int main() {
    exec();

    return 0;
}
