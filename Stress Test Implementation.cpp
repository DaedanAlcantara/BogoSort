#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>
#include <climits>
#include <iomanip>
#include <thread>
#include <future>
#include <atomic>
#include <signal.h>

// Assuming these headers exist and have the correct function signatures
#include "Stalin_Sort.h"
#include "Bogobogo_Sort.h"
#include "Sleep_Sort.h"
#include "Thanos_Sort.h"

using namespace std;

long long shuffleCount = 0;

// Global flag for timeout handling
atomic<bool> timeoutOccurred(false);

// ─────────────────────────────────────────────
//  Utility: print a vector (truncate if large)
// ─────────────────────────────────────────────
void printVector(const vector<int>& v, int maxShow = 10) {
    cout << "[";
    int limit = min((int)v.size(), maxShow);
    for (int i = 0; i < limit; i++) {
        cout << v[i];
        if (i < limit - 1) cout << ", ";
    }
    if ((int)v.size() > maxShow) cout << ", ... (" << v.size() - maxShow << " more)";
    cout << "]";
}

vector<int> generateRandomVector(int size, int minVal = 0, int maxVal = 100) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minVal, maxVal);

    vector<int> vec;
    vec.reserve(size);
    for (int i = 0; i < size; i++) {
        vec.push_back(dis(gen));
    }
    return vec;
}

vector<int> generateRandomVectorWithDuplicates(int size, int minVal = 0, int maxVal = 10) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minVal, maxVal);

    vector<int> vec;
    vec.reserve(size);
    for (int i = 0; i < size; i++) {
        vec.push_back(dis(gen));
    }
    return vec;
}

// ─────────────────────────────────────────────
//  UNIVERSAL TEST RUNNER WITH TIMEOUT
//  Can be used for ALL tests (regular and absurdist)
// ─────────────────────────────────────────────
bool runTestWithTimeout(const string& sorterName, const string& label,
                        vector<int> input,
                        vector<int> (*sorter)(vector<int>),
                        double timeoutSeconds,
                        bool verbose = true)
{
    if (verbose) {
        cout << "\n==========================================\n";
        cout << "  SORTER : " << sorterName;
        if (timeoutSeconds > 0) {
            cout << " (TIMEOUT: " << timeoutSeconds << "s)";
        }
        cout << "\n  TEST   : " << label << "\n";
        cout << "  Input  : "; printVector(input); cout << "\n";
        cout << "==========================================\n";
        cout.flush();
    }

    // If timeout is 0 or negative, run without timeout
    if (timeoutSeconds <= 0) {
        auto start = chrono::high_resolution_clock::now();
        vector<int> result = sorter(input);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        
        if (verbose) {
            cout << fixed << setprecision(6);
            cout << "  Output : "; printVector(result); cout << "\n";
            cout << "  Time   : " << elapsed.count() << " seconds\n";
            bool sorted = is_sorted(result.begin(), result.end());
            cout << "  Result : " << (sorted ? "SORTED" : "PARTIALLY SORTED") << "\n";
        }
        return true;
    }

    // Run with timeout
    packaged_task<vector<int>(vector<int>)> task(sorter);
    future<vector<int>> futureResult = task.get_future();

    auto start = chrono::high_resolution_clock::now();
    
    thread sortingThread(move(task), input);
    
    auto status = futureResult.wait_for(chrono::duration<double>(timeoutSeconds));
    
    bool completed = false;
    vector<int> result;
    
    if (status == future_status::ready) {
        completed = true;
        result = futureResult.get();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        
        if (verbose) {
            cout << fixed << setprecision(6);
            cout << "  Output : "; printVector(result); cout << "\n";
            cout << "  Time   : " << elapsed.count() << " seconds\n";
            bool sorted = is_sorted(result.begin(), result.end());
            cout << "  Result : " << (sorted ? "SORTED" : "PARTIALLY SORTED") << "\n";
        }
    } else {
        if (verbose) {
            cout << "  TIMEOUT! Operation exceeded " << timeoutSeconds << " seconds\n";
            cout << "  Killing the operation...\n";
        }
        // Detach the thread - it will run in background but we won't wait
        sortingThread.detach();
        timeoutOccurred = true;
    }
    
    if (sortingThread.joinable() && completed) {
        sortingThread.join();
    }
    
    if (verbose) {
        cout << "  Status : " << (completed ? "COMPLETED" : "TIMED OUT") << "\n";
        cout.flush();
    }
    
    return completed;
}

// ─────────────────────────────────────────────
//  Legacy wrapper for backward compatibility
// ─────────────────────────────────────────────
void runTest(const string& sorterName, const string& label,
             vector<int> input,
             vector<int> (*sorter)(vector<int>))
{
    // Run with a generous 60 second timeout for regular tests
    runTestWithTimeout(sorterName, label, input, sorter, 60.0, true);
}

// ─────────────────────────────────────────────
//  Wrappers to normalize each sorter's signature
// ─────────────────────────────────────────────
vector<int> runStalin(vector<int> arr) {
    return stalinSort<int>(arr);
}

vector<int> runBogobogo(vector<int> arr) {
    bogobogosort(arr, arr.size());
    return arr;
}

vector<int> runSleep(vector<int> arr) {
    return sleepSort(arr);
}

vector<int> runThanos(vector<int> arr) {
    return thanosSort(arr);
}

int main() {
    cout << "==============================================\n";
    cout << "   MULTI-ALGORITHM SORTING STRESS TEST\n";
    cout << "==============================================\n";
    cout << "  All tests have a 60-second timeout protection\n";
    cout << "==============================================\n\n";

    // Shared test cases
    vector<pair<string, vector<int>>> tests = {
        // Easy cases
        {"Already sorted", {1, 2, 3, 4, 5}},
        {"Reverse sorted", {5, 4, 3, 2, 1}},
        {"With duplicates", {3, 1, 2, 3, 1}},
        {"All equal", {2, 2, 2, 2, 2}},
        // Medium cases
        {"Random small", {10, 3, 5, 1, 4}},
        {"Random medium", {34, 23, 122, 9, 45, 67, 12}},
        {"With negatives", {3, -1, 2, -5, 0}},
        {"Mixed duplicates", {5, 3, 5, 1, 3}},
        // Hard cases
        {"Larger random", {10, 34, 23, 122, 9, 45, 67, 12, 89, 56}},
        {"Many duplicates", {2, 3, 2, 1, 3, 1, 2, 3}},
        {"Negative and positive", {-10, -5, 0, 5, 10}},
        // Edge cases 
        {"Empty array", {}},
        {"Single element", {42}},
        {"Two elements sorted", {1, 2}},
        {"Two elements reverse", {2, 1}},
    };

    // ── Stalin Sort ──────────────────────────────
    cout << "\n\n=== STALIN SORT ===\n";
    cout << "  Note: Eliminates out-of-order elements.\n";
    cout << "  Output may be shorter than input!\n";
    for (auto& [label, data] : tests) {
        runTest("Stalin Sort", label, data, runStalin);
    }

    // ── Bogobogo Sort ────────────────────────────
    cout << "\n\n=== BOGOBOGO SORT ===\n";
    cout << "  Note: Extremely slow — only small inputs used!\n";
    vector<pair<string, vector<int>>> bogoTests = {
        {"Already sorted", {1, 2, 3}},
        {"Reverse sorted", {3, 2, 1}},
        {"With duplicate", {2, 1, 2}},
        {"Single element", {42}},  // Added for completeness
        {"Empty array", {}},        // Added for completeness
    };
    for (auto& [label, data] : bogoTests) {
        runTest("Bogobogo Sort", label, data, runBogobogo);
    }

    // ── Sleep Sort ───────────────────────────────
    cout << "\n\n=== SLEEP SORT ===\n";
    cout << "  Note: Each number sleeps for its value in ms.\n";
    cout << "  Only works correctly with positive integers!\n";
    vector<pair<string, vector<int>>> sleepTests = {
        {"Small positives",  {4, 1, 3, 2}},
        {"Larger values",    {34, 23, 122, 9}},
        {"With duplicates",  {5, 3, 5, 1}},
        {"Single element",   {42}},  // Added
    };
    for (auto& [label, data] : sleepTests) {
        runTest("Sleep Sort", label, data, runSleep);
    }

    // ── Thanos Sort ──────────────────────────────
    cout << "\n\n=== THANOS SORT ===\n";
    cout << "  Note: Randomly eliminates half the array\n";
    cout << "  until what remains is sorted.\n";
    cout << "  WARNING: Empty array may cause infinite loop!\n";
    cout << "  Timeout protection is enabled (60 seconds).\n";
    
    for (auto& [label, data] : tests) {
        runTest("Thanos Sort", label, data, runThanos);
    }

    // ==============================================
    //  ABSURDIST TEST CASES WITH AGGRESSIVE TIMEOUTS
    // ==============================================
    cout << "\n\n==============================================\n";
    cout << "   ABSURDIST TEST CASES\n";
    cout << "==============================================\n";
    cout << "  These tests have shorter timeouts and\n";
    cout << "  will be killed if they take too long.\n\n";

    // Generate test vectors
    cout << "Generating test vectors...\n";
    cout.flush();
    
    vector<pair<string, vector<int>>> mediumTests = {
        {"Medium random", generateRandomVector(100)},
        {"Medium with duplicates", generateRandomVectorWithDuplicates(100)},
    };
    
    vector<pair<string, vector<int>>> largeTests = {
        {"Large random", generateRandomVector(1000)},
        {"Large with duplicates", generateRandomVectorWithDuplicates(1000)},
    };
    
    cout << "Vectors generated. Running tests...\n";
    cout.flush();

    // Medium tests (100 elements) with timeouts
    cout << "\n--- MEDIUM TESTS (100 elements) ---\n";
    
    cout << "\n[Stalin Sort - Timeout: 10 seconds]\n";
    for (auto& [label, data] : mediumTests) {
        runTestWithTimeout("Stalin Sort", label, data, runStalin, 60.0, true);
    }
    
    cout << "\n[Thanos Sort - Timeout: 30 seconds]\n";
    for (auto& [label, data] : mediumTests) {
        runTestWithTimeout("Thanos Sort", label, data, runThanos, 60.0, true);
    }
    
    cout << "\n[Sleep Sort - Timeout: 60 seconds]\n";
    cout << "  Note: Sleep sort must wait for values up to 100ms each\n";
    for (auto& [label, data] : mediumTests) {
        runTestWithTimeout("Sleep Sort", label, data, runSleep, 60.0, true);
    }
    
    cout << "\n[Bogobogo Sort - Timeout: 5 seconds]\n";
    cout << "  Note: Bogobogo will almost certainly timeout!\n";
    for (auto& [label, data] : mediumTests) {
        runTestWithTimeout("Bogobogo Sort", label, data, runBogobogo, 60.0, true);
    }

    // Large tests (1000 elements) with very aggressive timeouts
    cout << "\n\n--- LARGE TESTS (1000 elements) ---\n";
    cout << "  WARNING: These tests have very short timeouts\n";
    cout << "  and will likely all timeout except Stalin/Thanos.\n";
    
    cout << "\n[Stalin Sort - Timeout: 5 seconds]\n";
    for (auto& [label, data] : largeTests) {
        runTestWithTimeout("Stalin Sort", label, data, runStalin, 60.0, true);
    }
    
    cout << "\n[Thanos Sort - Timeout: 15 seconds]\n";
    for (auto& [label, data] : largeTests) {
        runTestWithTimeout("Thanos Sort", label, data, runThanos, 60.0, true);
    }
    
    cout << "\n[Sleep Sort - Timeout: 30 seconds]\n";
    cout << "  Note: With 1000 elements sleeping up to 100ms,\n";
    cout << "  this will likely timeout.\n";
    for (auto& [label, data] : largeTests) {
        runTestWithTimeout("Sleep Sort", label, data, runSleep, 60.0, true);
    }
    
    cout << "\n[Bogobogo Sort - Timeout: 2 seconds]\n";
    cout << "  Note: Bogobogo will timeout immediately!\n";
    for (auto& [label, data] : largeTests) {
        runTestWithTimeout("Bogobogo Sort", label, data, runBogobogo, 60.0, true);
    }
    
    // Special test: Thanos on empty array with short timeout to demonstrate protection
    cout << "\n\n--- EDGE CASE TEST: Thanos Sort on Empty Array ---\n";
    cout << "  This test demonstrates timeout protection.\n";
    vector<int> emptyVec = {};
    runTestWithTimeout("Thanos Sort", "Empty array (timeout test)", emptyVec, runThanos, 60.0, true);
    
    cout << "\n\n==============================================\n";
    cout << "             ALL TESTS COMPLETE\n";
    cout << "==============================================\n";
    
    if (timeoutOccurred) {
        cout << "  ⚠️  Some tests timed out and were killed.\n";
        cout << "  This is expected for slow sorts or buggy implementations.\n";
    }
    
    cout << "==============================================\n";

    return 0;
}