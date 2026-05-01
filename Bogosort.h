#pragma once
#include <vector>
#include <algorithm>
#include <random>

inline bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

inline void shuffleArray(std::vector<int>& arr) {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
}

inline void bogosort(std::vector<int>& arr) {
    while (!isSorted(arr)) {
        shuffleArray(arr);
    }
}