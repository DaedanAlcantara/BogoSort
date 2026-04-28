#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>
#include <climits>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────
//  Global shuffle counter (reset per test)
// ─────────────────────────────────────────────
long long shuffleCount = 0;

void shuffleArray(vector<int>& arr, int n) {
    random_device rd;
    mt19937 g(rd());
    shuffle(arr.begin(), arr.begin() + n, g);
    shuffleCount++;
}

void bogobogosort(vector<int>& arr, int n) {
    if (n <= 1) return;
    while (true) {
        bogobogosort(arr, n - 1);
        if (arr[n - 1] >= arr[n - 2]) return;
        shuffleArray(arr, n);
    }
}

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

// ─────────────────────────────────────────────
//  Run a single test case with timeout guard
// ─────────────────────────────────────────────
void runTest(const string& label, vector<int> data, double timeoutSeconds = 30.0) {
    cout << "\n╔══════════════════════════════════════════════╗\n";
    cout << "  TEST: " << label << "\n";
    cout << "  Input size : " << data.size() << "\n";
    cout << "  Input      : "; printVector(data); cout << "\n";
    cout << "╚══════════════════════════════════════════════╝\n";

    shuffleCount = 0;

    auto start = chrono::high_resolution_clock::now();
    bogobogosort(data, data.size());
    auto end   = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    cout << "  Output     : "; printVector(data); cout << "\n";
    cout << fixed << setprecision(6);
    cout << "  Time       : " << elapsed.count() << " seconds\n";
    cout << "  Shuffles   : " << shuffleCount << "\n";

    bool sorted = is_sorted(data.begin(), data.end());
    cout << "  Verified   : " << (sorted ? "✔ SORTED" : "✘ NOT SORTED") << "\n";
}

int main() {

    cout << "==============================================\n";
    cout << "      BOGOBOGOSORT — COMPREHENSIVE STRESS TEST\n";
    cout << "==============================================\n";

    // ─────────────────────────────────────────
    // TIER 0: TRIVIAL / BASE CASES
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 0: TRIVIAL / BASE CASES ━━━━\n";

    runTest("Empty array",                  {});
    runTest("Single element",               {99});
    runTest("Two elements — sorted",        {1, 2});
    runTest("Two elements — reversed",      {2, 1});
    runTest("Three elements — pre-sorted",  {1, 2, 3});

    // ─────────────────────────────────────────
    // TIER 1: DUPLICATE ELEMENTS
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 1: DUPLICATE ELEMENTS ━━━━\n";

    runTest("All identical",                {5, 5, 5});
    runTest("Two distinct values",          {1, 2, 1, 2, 1});
    runTest("One outlier, rest same",       {7, 7, 7, 1, 7});
    runTest("Heavy duplicates (reversed)",  {3, 3, 2, 2, 1, 1});
    runTest("All same — 4 elements",        {9, 9, 9, 9});

    // ─────────────────────────────────────────
    // TIER 2: ZEROS & SIGN MIXING
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 2: ZEROS & SIGN MIXING ━━━━\n";

    runTest("All zeros",                    {0, 0, 0});
    runTest("Zeros and positives",          {0, 3, 0, 1});
    runTest("Zeros and negatives",          {0, -1, 0, -3});
    runTest("Negative, zero, positive",     {-2, 0, 2});
    runTest("Mixed signs — reversed order", {3, 1, 0, -1, -3});

    // ─────────────────────────────────────────
    // TIER 3: NEGATIVE NUMBERS
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 3: NEGATIVE NUMBERS ━━━━\n";

    runTest("All negatives — pre-sorted",   {-3, -2, -1});
    runTest("All negatives — reversed",     {-1, -2, -3});
    runTest("All negatives — random order", {-5, -1, -4, -2});

    // ─────────────────────────────────────────
    // TIER 4: EXTREME VALUES
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 4: EXTREME INTEGER VALUES ━━━━\n";

    runTest("INT_MAX and INT_MIN",          {INT_MAX, INT_MIN, 0});
    runTest("Max boundary cluster",         {INT_MAX, INT_MAX - 1, INT_MAX - 2});
    runTest("Min boundary cluster",         {INT_MIN + 2, INT_MIN + 1, INT_MIN});
    runTest("Large value spread",           {1000000, -1000000, 0, 500000});

    // ─────────────────────────────────────────
    // TIER 5: ORDERING EXTREMES
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 5: ORDERING EXTREMES ━━━━\n";

    runTest("Already sorted — 4 el",        {1, 2, 3, 4});
    runTest("Reverse sorted — 4 el",        {4, 3, 2, 1});
    runTest("Almost sorted (1 swap) — 4 el",{1, 3, 2, 4});
    runTest("Alternating high-low — 4 el",  {4, 1, 3, 2});
    runTest("Reverse sorted — 5 el ⚠",     {5, 4, 3, 2, 1});

    // ─────────────────────────────────────────
    // TIER 6: RANDOM PERMUTATIONS (re-seeded)
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 6: RANDOM PERMUTATIONS ━━━━\n";

    mt19937 rng(42); // fixed seed for reproducibility
    auto makeRandom = [&](int n, int lo, int hi) {
        uniform_int_distribution<int> dist(lo, hi);
        vector<int> v(n);
        for (auto& x : v) x = dist(rng);
        return v;
    };

    runTest("Random 3-el, range [0,9]",     makeRandom(3, 0, 9));
    runTest("Random 3-el, range [-9,9]",    makeRandom(3, -9, 9));
    runTest("Random 4-el, range [0,99]",    makeRandom(4, 0, 99));
    runTest("Random 4-el, all dupes possible", makeRandom(4, 1, 2));

    // ─────────────────────────────────────────
    // TIER 7: DANGER ZONE (5-element cases)
    // WARNING: These may take minutes or longer.
    // Comment out any you do not wish to run.
    // ─────────────────────────────────────────
    cout << "\n\n━━━━ TIER 7: DANGER ZONE — 5 ELEMENTS ⚠ ━━━━\n";
    cout << "  (These can take seconds to minutes. Be patient.)\n";

    runTest("5-el sorted",                  {1, 2, 3, 4, 5});
    runTest("5-el reverse-sorted ☠",        {5, 4, 3, 2, 1});
    runTest("5-el random",                  makeRandom(5, 0, 9));
    runTest("5-el all duplicates",          {3, 3, 3, 3, 3});
    runTest("5-el zeros and ones",          {1, 0, 1, 0, 1});

    cout << "\n\n==============================================\n";
    cout << "             ALL TESTS COMPLETE\n";
    cout << "==============================================\n";

    return 0;
}