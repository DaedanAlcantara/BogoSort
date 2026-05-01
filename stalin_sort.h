#pragma once
#include <vector>

template <typename T>
std::vector<T> stalinSort(const std::vector<T>& arr) {
    if (arr.empty()) return {};

    std::vector<T> ordered;
    ordered.push_back(arr[0]);

    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] >= ordered.back()) {
            ordered.push_back(arr[i]);
        }
    }
    return ordered;
}
