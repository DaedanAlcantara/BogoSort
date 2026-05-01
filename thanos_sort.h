#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <chrono>

inline std::vector<int> thanosSort(std::vector<int> arr) {
    int n = arr.size();

    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);

    while (true) {
        int i = 0;
        while (i < n - 1 && arr[i] <= arr[i + 1]) i++;

        if (i == n - 1) return arr;

        int keepCount = n / 2;
        std::unordered_set<int> indices;

        while ((int)indices.size() < keepCount) {
            std::uniform_int_distribution<int> dist(0, n - 1);
            indices.insert(dist(rng));
        }

        std::vector<int> newArr;
        for (int idx : indices) newArr.push_back(arr[idx]);

        arr = newArr;
        n = arr.size();
    }
}
