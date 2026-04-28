#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// Helper function to shuffle only the first 'n' elements
void shuffleArray(vector<int>& arr, int n) {
    random_device rd;
    mt19937 g(rd());
    shuffle(arr.begin(), arr.begin() + n, g);
}

// The raw Bogobogosort algorithm
void bogobogosort(vector<int>& arr, int n) {
    if (n <= 1) return;

    while (true) {
        bogobogosort(arr, n - 1);

        if (arr[n - 1] >= arr[n - 2]) {
            return;
        }

        shuffleArray(arr, n);
    }
}

int main() {
    vector<int> data = {3, 1, 2};

    auto start = chrono::high_resolution_clock::now();

    bogobogosort(data, data.size());

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    for (int x : data) {
        cout << x << " ";
    }
    cout << endl;

    cout << "Execution time: " << elapsed.count() << " seconds" << endl;

    return 0;
}
