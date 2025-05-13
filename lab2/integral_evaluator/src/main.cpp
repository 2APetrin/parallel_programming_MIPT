#include "integral_evaluator.hpp"

#include <stdexcept>
#include <iostream>
#include <string>
#include <cmath>

#include <chrono>

double func(double x) { return std::sin(1/x);  }
double test(double x) { return std::exp(-x*x); }

int main(int argc, char *argv[]) {
    namespace ie = integral_evaluator;

    if (argc < 5) throw std::runtime_error("bag arg count");
    double a         = std::stod(argv[1]);
    double b         = std::stod(argv[2]);
    double accuracy  = std::stod(argv[3]);
    int    n_workers = std::stoi(argv[4]);

    if (a > b) throw std::runtime_error("a > b pupupu...");
    if (accuracy > 1 || accuracy < 0) throw std::runtime_error("accuracy should be in range (0, 1) ");

    ie::integrator intgrtr(func, a, b, accuracy, n_workers);

    auto beg = std::chrono::high_resolution_clock::now();
    double result = intgrtr.get_answer();
    auto end = std::chrono::high_resolution_clock::now();
    auto time = (double) std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg).count() / 1000000;

    std::cout.precision(15);
    std::cout << "parallel intergal value = " << result << ", time = " << time << " ms" << std::endl;

    return 0;
}
