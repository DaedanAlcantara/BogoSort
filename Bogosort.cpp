#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

// Helper function to verify if the array is sorted
bool isSorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

// Helper function to shuffle the array
void shuffleArray(vector<int>& arr) {
    static random_device rd;
    static mt19937 g(rd());  // setting up a high-quality random number generator 
    shuffle(arr.begin(), arr.end(), g);
}

// The raw Bogosort algorithm
void bogosort(vector<int>& arr) {
    while (!isSorted(arr)) {
        shuffleArray(arr);
    }
}

int main() {
    vector<int> data;
    int input, count;

    cout << "How many numbers do you want to sort?: ";
    cin >> count;

    cout << "Enter " << count << " integers: ";
    for(int i = 0; i < count; i++) {
        cin >> input;
        data.push_back(input);
    }

    bogosort(data); 
        
    cout << "Sorted result: ";
    for(int x : data) {
        cout << x << " ";
    }
    
    return 0;
}
