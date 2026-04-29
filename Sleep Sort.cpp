#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex coutMutex;

void sleepSort(const std::vector<int>& arr) {
    std::vector<std::thread> workers;
    workers.reserve(arr.size());

    for (int num : arr) {
        workers.emplace_back([num]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(num));
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << num << ' ';
        });
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    std::cout << std::endl;
}

int main() {
    std::vector<int> arr{34, 23, 122, 9};
    sleepSort(arr);
    return 0;
}
