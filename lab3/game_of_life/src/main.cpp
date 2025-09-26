#ifdef SIMULATION
    #include <iostream>
#endif // SIMULATION
#include "core/Board.hpp"
#include <omp.h>

int main(int argc, char* argv[]) {
    omp_set_num_threads(2);
    GameOfLife::Board b;
    b.load_state("./initial_state.txt");
    b.save_current_state("./tmp.json");

#ifdef PARALLEL
    #pragma omp parallel
    #pragma omp single
    {
#endif // PARALLEL

#ifdef SIMULATION
    std::string line;
    while(std::getline(std::cin, line)) {
        b.next_generation();
        b.save_current_state("./tmp.json");
        std::cout << "done" << std::endl;
    }
#else
    int cnt = std::stoi(argv[1]);
    #ifdef PARALLEL
        for (int i = 0; i < cnt; ++i) b.next_generation_parallel();
    #else
        for (int i = 0; i < cnt; ++i) b.next_generation();
    #endif // PARALLEL
#endif // SIMULATION

#ifdef PARALLEL
    }
#endif // PARALLEL
    return 0;
}
