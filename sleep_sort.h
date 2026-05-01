#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

inline std::vector<int> sleepSort(const std::vector<int>& arr) {
    std::vector<std::thread> workers;
    std::vector<int> result;
    std::mutex resultMutex;

    workers.reserve(arr.size());

    for (int num : arr) {
        workers.emplace_back([num, &result, &resultMutex]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(num));
            std::lock_guard<std::mutex> lock(resultMutex);
            result.push_back(num);
        });
    }

    for (auto& worker : workers) {
        if (worker.joinable()) worker.join();
    }

    return result;
}
