#include <vector>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <chrono>

std::vector<int> thanosSort(std::vector<int> arr) {
    std::vector<int> A = arr;
    int n = A.size();
    
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);
    
    while (true) {
        int i = 0;
        
        while (i < n - 1 && A[i] <= A[i + 1]) {
            i++;
        }
        
        if (i == n - 1) {
            return std::vector<int>(A.begin(), A.begin() + n);
        }
        
        int keepCount = n / 2;  // floor division for integers
        std::unordered_set<int> indices;
        
        while (indices.size() < static_cast<size_t>(keepCount)) {
            std::uniform_int_distribution<int> dist(0, n - 1);
            indices.insert(dist(rng));
        }
        
        std::vector<int> newArr;
        for (int idx : indices) {
            newArr.push_back(A[idx]);
        }
        
        A = newArr;
        n = A.size();
    }
}
