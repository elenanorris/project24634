#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <cstring> // for strcmp
#include <fstream>
#include <chrono>
using namespace std;

vector<chrono::duration<double>> histogramArray; // stores the computed times of the Collatz sequence for specific integers n from collatzCompute()
mutex histMutex; // ensures only one thread accesses a shared resource at a time
bool locking = true;

/*
collatzCompute() measures the time it takes to compute a Collatz sequence
of a given integer n, and it then stores that time in the histogramArray vector.
*/
void collatzCompute(long long n, size_t index) {
    auto start = chrono::high_resolution_clock::now(); // Starts measurement of Collatz sequence computation

    while (n != 1) { // Base case for Collatz sequence
        if (n % 2 == 1) { // If n is odd
            n = 3 * n + 1;
        }
        else { // If n is even
            n = n / 2;
        }
    }
    
    auto end = chrono::high_resolution_clock::now(); // Ends measurement of Collatz sequence computation
    auto collatzComputationTime = end-start; // Computes time it took for collatzComputation to complete

    if (locking) { //avoid race conditions
        lock_guard<mutex> lock(histMutex);
        histogramArray[index] = collatzComputationTime;
    } else {
        histogramArray[index] = collatzComputationTime; // need unique indices to keep threads safe
    }
}

/*
Writes all the times that are stored in histogramArray to a .csv filed named collatzTime.csv
*/
void writeCSV() {
    ofstream fout("collatzTime.csv");
    if (!fout.is_open()){
        cerr << "Error: Failed to open collatzTime.csv" << endl;
        return;
    }
    for (const auto& time : histogramArray){
        fout << time.count() << "\n";
    }
}

int main(int args, char* argv[]) {
    if (args < 3) {
        cerr << "Usage: ./mt-collatz N T [-nolock]" << endl; // Tells user how to properly execute program in terminal
        return 1;
    }
    long N = stol(argv[1]);
    int T = stoi(argv[2]);

    if (args ==4 && strcmp(argv[3], "-nolock") == 0) { //checks for lock argument
        locking = false;
    }

    // Trivial case handling
    if (T <= 0) {
        T = 1;
    }
    if (N <= 0) {
        return 0; // end program outright
    }
    if (T > N) { // avoids creating more threads than units of work
        T = N;
    }

    histogramArray.resize(N); // must pre-allocate vector to avoid segmentation fault when running with -nolock
    auto histogramArrayStart = chrono::high_resolution_clock::now();
    
    // Computing chunk sizes for dividing workload of sequence computation among threads 
    int base = N / T;
    int remainder = N % T;
    vector<thread> threads;
    threads.reserve(T);

    // Create T threads, with each thread computing a distinct chunk of the sequence [0, N-1]
    for (int threadID = 0; threadID < T; threadID++) {
        int start = (threadID * base) + min(threadID, remainder); // compute where thread's chunk starts in sequence
        int count; // how many elements thread will compute
        if (threadID < remainder) {
            count = base + 1; // threads with threadID < remainder will get one extra element to cover the remainder
        }
        else {
            count = base;
        }
        int end = start + count; // end index for reach thread

        // Have threads process to compute their Collatz sequence chunks and store their times in histogramArray
        threads.emplace_back([start, end]() {
            for (int i = start; i < end; i++) {
                collatzCompute(i + 1); // each thread processes its chunk. Collatz are 
            }
            });

        
    }

    // Joining all threads
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    auto histogramArrayEnd = chrono::high_resolution_clock::now();
    auto histogramArrayTime = histogramArrayEnd - histogramArrayStart; // used for measuring the total time to population histogramArray with the typed in inputs

    if (histogramArray.size() == 0) {
        cerr << "An error occurred with storing computation times inside the histogramArray. It is empty." << endl;
    }
    
    writeCSV();

    cout << "Completed " << N << " computations with " << T << " threads" << endl;
    cout << "Locking: " << (locking ? "enabled" : "disabled") << endl;
    cout << "Time: " << histogramArrayTime.count() << "seconds" << endl;

    return 0;
}



/*
Sources:
[1] C++ Multithreading and Concurrency Tutorial: https://www.educative.io/blog/modern-multithreading-and-concurrency-in-cpp
[2] Multithreading in C++ Pt. 1: https://www.geeksforgeeks.org/cpp/multithreading-in-cpp/
[3] Multithreading in C++ Pt. 2: https://www.educative.io/courses/modern-cpp-concurrency-in-practice-get-the-most-out-of-any-machine/multithreading-in-cpp
[4] Collatz sequence in C++: https://www.geeksforgeeks.org/dsa/program-to-print-collatz-sequence/
[5] Multithreading vs. Multiprocessing (with Python): https://builtin.com/data-science/multithreading-multiprocessing
[6] .reserve() method for C++ vectors: https://en.cppreference.com/w/cpp/container/vector/reserve
[7] Threading with chunking: https://stackoverflow.com/questions/49368207/using-threading-to-slice-an-array-into-chunks-and-perform-calculation-on-each-ch
[8] .emplace_back() for C++ vectors: https://en.cppreference.com/w/cpp/container/vector/emplace_back
[9] Lambda functions in C++: https://www.w3schools.com/cpp/cpp_functions_lambda.asp
[10] Mutex lock for thread synchronization: https://www.geeksforgeeks.org/linux-unix/mutex-lock-for-linux-thread-synchronization/
[11] Writing elements of a vector to a .CSV file in C++: https://www.geeksforgeeks.org/cpp/csv-file-management-using-c/


*/



