#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
using namespace std;

vector<chrono::duration<double>> histogramArray; // stores the computed times of the Collatz sequence for specific integers n from collatzCompute()
int COUNTER = 1; // used for thread management

/*
Below is the helper function for collatzCompute().
It returns the duration of an operation by computing the difference
between the end and start times of collatzCompute().
*/ 
chrono::duration<double> getDuration(
    chrono::time_point<chrono::high_resolution_clock> start,
    chrono::time_point<chrono::high_resolution_clock> end)
{
    chrono::duration<double> duration = end - start;
    return duration;
}

/*
collatzCompute() measures the time it takes to compute a Collatz sequence
of a given integer n, and it then stores that time in the histogramArray vector.
*/
void collatzCompute(int n) {
    // Starts measurement of Collatz sequence computation
    auto start = chrono::high_resolution_clock::now();

    while (n != 1) { // Base case for Collatz sequence
        if (n % 2 == 1) { // If n is odd
            n = 3 * n + 1;
        }
        else { // If n is even
            n = n / 2;
        }
    }
    // end of collatz sequence computation

    // Ends measurement of Collatz sequence computation
    auto end = chrono::high_resolution_clock::now();

    // Computes time it took for collatzComputation to complete
    chrono::duration<double> collatzComputationTime = getDuration(start, end);
    
    // Stores the collatzComputation time in histogramArray vector
    histogramArray.push_back(collatzComputationTime);
}

int main() {
    auto histogramArrayStart = chrono::high_resolution_clock::now();
    int T = 4; // num of threads
    int N = 10; // range of numbers for a Collatz sequence to be computed

    vector<thread> threads;


    while (COUNTER <= N) {
        for (int i = 0; i < T; i++) {
            threads.push_back(thread(collatzCompute, COUNTER));
            COUNTER++;
        }
        cout << "# Threads: " << threads.size() << endl;
    }
    for (auto& th : threads) {
        th.join();
    }

    auto histogramArrayEnd = chrono::high_resolution_clock::now();
    // Print elapsed time of program execution
    // Print histogramArray
    //Terminate programs and threads



    // testing: prints times in histogramArray
    for (int i = 0; i < histogramArray.size(); i++) { 
        cout << "Collatz Computation Time: " << histogramArray[i].count() << endl; 
    } 

    return 0;
}



/*
Sources:
[1] C++ Multithreading and Concurrency Tutorial: https://www.educative.io/blog/modern-multithreading-and-concurrency-in-cpp
[2] Multithreading in C++ Pt. 1: https://www.geeksforgeeks.org/cpp/multithreading-in-cpp/
[3] Multithreading in C++ Pt. 2: https://www.educative.io/courses/modern-cpp-concurrency-in-practice-get-the-most-out-of-any-machine/multithreading-in-cpp
[4] Collatz sequence in C++: https://www.geeksforgeeks.org/dsa/program-to-print-collatz-sequence/
[5] Multithreading vs. Multiprocessing (with Python): https://builtin.com/data-science/multithreading-multiprocessing
*/

// Issue: Triple the amount of threads is being made in the While loop.
// Only want 4 threads, is making 12 threads total with current inputs.