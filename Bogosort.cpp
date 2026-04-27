#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

// Helper function to shuffle only the first 'n' elements
void shuffleArray(vector<int>& arr, int n) {
    random_device rd;
    mt19937 g(rd()); // setting up a high-quality random number generator 
    shuffle(arr.begin(), arr.begin() + n, g);
}

// The raw Bogobogosort algorithm
void bogobogosort(vector<int>& arr, int n) {
    // 1. Base case: 1 or 0 elements are already sorted
    if (n <= 1) return; 

    while (true) {
        // 2. Recursively sort the first n-1 elements
        bogobogosort(arr, n - 1); 

        // 3. Check if the last element is greater than or equal to the one before it
        if (arr[n - 1] >= arr[n - 2]) {
            return; // The whole subset is sorted!
        }
        
        // 4. If not, shuffle the subset and try again
        shuffleArray(arr, n);
    }
}

int main() {
    // A tiny array to test it (keep it small, e.g., 3 or 4 items)
    vector<int> data = {3, 1, 2}; 
    
    bogobogosort(data, data.size());
    
    // Print the result
    for(int x : data) {
        cout << x << " ";
    }
    
    return 0;
}
