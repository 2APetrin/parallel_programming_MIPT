#include "Board.hpp"
#include "BoardSerializer.hpp"

using namespace GameOfLife;
using namespace Detail;

std::string JsonSerizlizer::serialize(const Board& b) const {
    std::string json = "{\"alive_cells\":[\n";
    auto pts = b.get_current_state().alive_pts_;

    bool first = true;
    for (auto&& e : pts) {
        if (!first) {
            json += ",\n";
        }
        json += "{\"x\":" + std::to_string(e.x) +
                ",\"y\":" + std::to_string(e.y) + "}";
        first = false;
    }
    json += "]}";

    return json;
}
