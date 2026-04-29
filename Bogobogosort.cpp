#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

void shuffleArray(vector<int>& arr, int n) {
    random_device rd;
    mt19937 g(rd()); // setting up a high-quality random number generator 
    shuffle(arr.begin(), arr.begin() + n, g);
}

void bogobogosort(vector<int>& arr, int n) {
    if (n <= 1) return; 

    while (true) {
        // 2. Recursively sort the first n-1 elements
        bogobogosort(arr, n - 1); 

        if (arr[n - 1] >= arr[n - 2]) {
            return; // The whole subset is sorted!
        }
        
        shuffleArray(arr, n);
    }
}

int main() {
    vector<int> data = {3, 1, 2}; 
    
    bogobogosort(data, data.size());
    
    // Print the result
    for(int x : data) {
        cout << x << " ";
    }
    
    return 0;
}
