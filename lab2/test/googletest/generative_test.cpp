#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <chrono>

#include "bitonic_sort.hpp"

namespace bs = bitonic_sort;

class bitonic_sort_generative_test : public ::testing::TestWithParam<int> {
protected:
    void SetUp() override {
        int pow = GetParam();
        int size = std::pow(2, pow);
        std::srand(std::time(nullptr));

        for (int i = 0; i < size; ++i) array.push_back((std::rand() % size) - size / 2);
        array_copy = array;
    }

    std::vector<int> array;
    std::vector<int> array_copy;
};

TEST_P(bitonic_sort_generative_test, generative_test) {
    auto arr_copy1 = array;
    auto arr_copy2 = array;

    // warmup
    bs::bitonic_sort(arr_copy1.begin(), arr_copy1.end(), 8, std::less<int>());

    auto start_bitonic = std::chrono::high_resolution_clock::now();
    bs::bitonic_sort(array.begin(), array.end(), 8, std::less<int>());
    auto end_bitonic = std::chrono::high_resolution_clock::now();
    auto time_bitonic = std::chrono::duration_cast<std::chrono::nanoseconds>(end_bitonic - start_bitonic).count();
    std::cout << "bitonic time = " << (double)time_bitonic/1000000 << " ms" << std::endl;

    // warmup
    std::sort(arr_copy2.begin(), arr_copy2.end(), std::less<int>());

    auto start_stl = std::chrono::high_resolution_clock::now();
    std::sort(array_copy.begin(), array_copy.end(), std::less<int>());
    auto end_stl = std::chrono::high_resolution_clock::now();
    auto time_stl = std::chrono::duration_cast<std::chrono::nanoseconds>(end_stl - start_stl).count();
    std::cout << "stl time     = " << (double)time_stl/1000000 << " ms" << std::endl;

    ASSERT_TRUE(array == array_copy);
}

INSTANTIATE_TEST_SUITE_P(
    generative_suite,
    bitonic_sort_generative_test,
    ::testing::Range(10, 25),
    [](const testing::TestParamInfo<int>& info) {
      return std::to_string(info.param);
    }
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
