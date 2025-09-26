#include "Board.hpp"

#include <omp.h>

#include <iterator>
#include <unordered_set>
#include <fstream>
#include <utility>
#include <vector>

using namespace GameOfLife;

using Point = Detail::State::Point;
using PointSet = std::unordered_set<Point, Point::Hash>;

void Board::next_generation() {
    next_st_.clear();

    auto neighbors = PointSet();

    for (auto&& p : curr_st_.alive_pts_) {
        int neighbors_cnt = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if (x == 0 && y == 0) continue;
                if (curr_st_.contains(p.x + x, p.y + y)) ++neighbors_cnt;
                neighbors.emplace(p.x + x, p.y + y);
            }
        }
        if (neighbors_cnt > 1 && neighbors_cnt < 4) next_st_.emplace(p);
    }

    for (auto&& p : neighbors) {
        int neighbors_cnt = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if (x == 0 && y == 0) continue;
                if (curr_st_.contains(p.x + x, p.y + y)) ++neighbors_cnt;
            }
        }
        if (neighbors_cnt == 3) next_st_.emplace(p);
    }

    std::swap(next_st_, curr_st_);
}

void Board::save_current_state(const std::string& filename) const {
    std::ofstream file(filename);
    file << serializer_->serialize(*this);
    file.close();
}

void Board::load_state(const std::string& filename) {
    std::ifstream file(filename);

    std::string x;
    std::string y;

    curr_st_.clear();
    while (file >> x >> y) {
        curr_st_.emplace(std::stoi(x), std::stoi(y));
    }
    file.close();
}

void Board::next_generation_parallel() {
    next_st_.clear();

    std::vector v = {curr_st_.alive_pts_.begin()};
    auto pos = curr_st_.alive_pts_.begin();
    int max_trd = omp_get_max_threads();
    int sz = curr_st_.alive_pts_.size();
    int one_more = sz % max_trd;

    for (int i = 0; i < max_trd; ++i) {
        int to_add = sz / max_trd + (one_more-- > 0 ? 1 : 0);
        if (to_add == 0) continue;;
        std::advance(pos, to_add);
        v.emplace_back(pos);
    }

    for (auto it = v.begin(), end = --(v.end()); it != end; ++it) {
        #pragma omp task
        process_points(*it, *std::next(it));
    }

    #pragma omp taskwait

    std::swap(next_st_, curr_st_);
}

void Board::process_points(const PointSet::iterator beg, const PointSet::iterator end) {
    auto neighbors = PointSet();
    auto next = PointSet();

    for (auto it = beg; it != end; ++it) {
        auto p = *it;
        int neighbors_cnt = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if (x == 0 && y == 0) continue;
                if (curr_st_.contains(p.x + x, p.y + y)) ++neighbors_cnt;
                neighbors.emplace(p.x + x, p.y + y);
            }
        }
        if (neighbors_cnt > 1 && neighbors_cnt < 4) next.emplace(p);
    }

    for (auto&& p : neighbors) {
        int neighbors_cnt = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if (x == 0 && y == 0) continue;
                if (curr_st_.contains(p.x + x, p.y + y)) ++neighbors_cnt;
            }
        }
        if (neighbors_cnt == 3) next.emplace(p);
    }

    #pragma omp critical
    next_st_.alive_pts_.insert(next.begin(), next.end());
}
