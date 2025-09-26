#ifndef GAME_OF_LIFE_BOARD_HPP
#define GAME_OF_LIFE_BOARD_HPP

#include "BoardSerializer.hpp"
#include <unordered_set>
#include <memory>

namespace GameOfLife {

namespace Detail {

struct State {
    struct Point {
        int x;
        int y;

        bool operator==(const Point& rhs) const noexcept {
            return x == rhs.x && y == rhs.y;
        }

        struct Hash {
            std::size_t operator()(const Point& p) const {
                return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
            }
        };
    };

    using PointSet = std::unordered_set<Point, Point::Hash>;

public:
    PointSet alive_pts_;

    void emplace(int x, int y) {
        alive_pts_.emplace(x, y);
    }

    void emplace(Point p) {
        alive_pts_.emplace(p);
    }

    bool contains(int x, int y) const {
        return alive_pts_.contains(Point{x, y});
    }

    bool contains(const Point p) const {
        return alive_pts_.contains(p);
    }

    void erase(int x, int y) {
        alive_pts_.erase(Point{x, y});
    }

    void clear() {
        alive_pts_.clear();
    }
};

} // namespace Detail

class Board {
    using Point = Detail::State::Point;
    using PointSet = Detail::State::PointSet;

    Detail::State curr_st_;
    Detail::State next_st_;
    std::unique_ptr<Detail::AbstractBoardSerializer> serializer_ = std::make_unique<Detail::JsonSerizlizer>();

public:
    void next_generation();

    void next_generation_parallel();

    void set_point(int x, int y, bool is_alive) {
        if (is_alive) {
            curr_st_.emplace(x, y);
        }
        else {
            curr_st_.erase(x, y);
        }
    }

    std::string serialize() const {
        return serializer_->serialize(*this);
    }

    const Detail::State& get_current_state() const {
        return curr_st_;
    }

    void save_current_state(const std::string& filename) const;
    void load_state(const std::string& filename);

private:
    // void process_points(const std::unordered_set<Point, Point::Hash>& pts);
    void process_points(const PointSet::iterator beg, const PointSet::iterator end);
    void process_point(const Point& p, PointSet& pts);
};

} // namespace GameOfLife

#endif // GAME_OF_LIFE_BOARD_HPP
