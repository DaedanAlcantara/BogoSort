#pragma once
#include <vector>
#include <algorithm>
#include <random>

inline void _bogo_shuffle(std::vector<int>& arr, int n) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.begin() + n, g);
}

inline void bogobogosort(std::vector<int>& arr, int n) {
    if (n <= 1) return;

    while (true) {
        bogobogosort(arr, n - 1);
        if (arr[n - 1] >= arr[n - 2]) return;
        _bogo_shuffle(arr, n);
    }
}
