#ifndef GAME_OF_LIFE_BOARD_SERIALIZER_HPP
#define GAME_OF_LIFE_BOARD_SERIALIZER_HPP

#include <string>

namespace GameOfLife {

class Board;

namespace Detail {

class AbstractBoardSerializer {
public:
    virtual std::string serialize(const Board& b) const = 0;
    virtual ~AbstractBoardSerializer() {};
};

class JsonSerizlizer final : public AbstractBoardSerializer {
public:
    JsonSerizlizer() = default;
    std::string serialize(const Board& b) const;
};

} // namespace Detail

} // namespace GameOfLife

#endif // GAME_OF_LIFE_BOARD_SERIALIZER_HPP
