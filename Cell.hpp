#ifndef CELL_HPP_
#define CELL_HPP_

#include <cstdint>
#include <array>

enum Directions
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

struct Cell
{
    std::array<int32_t, 4> edge_id = {0, 0, 0, 0};
    std::array<bool, 4> edge_exists = {0, 0, 0, 0};
    bool exists = false;
};

#endif // CELL_HPP_