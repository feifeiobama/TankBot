//
// Created by szc on 2018/10/16.
//

#ifndef TANK_DEF_HPP
#define TANK_DEF_HPP

#define __STDC_LIMIT_MACROS

#ifndef _BOTZONE_ONLINE
//#define _BOTZONE_ONLINE
#endif

#include <cstdint>
#include <limits>
#include <climits>

struct Position {
    int x, y;
};

constexpr Position Null_pos = Position{-1, -1};

typedef int Move;
/*
    STILL = -1,
    MOVE_UP = 0,
    MOVE_RIGHT = 1,
    MOVE_DOWN = 2,
    MOVE_LEFT = 3,
    FIRE_UP = 4,
    FIRE_RIGHT = 5,
    FIRE_DOWN = 6,
    FIRE_LEFT = 7
*/

struct Action {
    Move move[2];
};

enum Color {
    BLUE = 0,
    RED = 1
};

constexpr int Dir[][2] = {{0,  -1},
                          {1,  0},
                          {0,  1},
                          {-1, 0}};

inline Color op_color(Color color) {
    return Color(1 - color);
}

inline bool is_in_field(const Position &position) {
    return position.x >= 0 && position.x < 9 && position.y >= 0 && position.y < 9;
}

inline Position get_adjacent_position(const Position &position, int i) {
    return Position{position.x + Dir[i][0], position.y + Dir[i][1]};
}

inline bool operator==(const Position &pos1, const Position &pos2) {
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

inline bool operator!=(const Position &pos1, const Position &pos2) {
    return pos1.x != pos2.x || pos1.y != pos2.y;
}

#define FOR_ADJACENT_POS_INDEX(position, adj_pos, expression)\
for (int dir = 0; dir != 4; ++dir) {\
    Position adj_pos = get_adjacent_position(position, dir);\
    if (is_in_field(adj_pos)) {\
        expression\
    }\
}

#define FOR_INIT_HALF_FIELD(i, j, expression)\
for (int i = 0; i != 4; ++i) {\
    for (int j = 0; j != 9; ++j) {\
        expression\
    }\
}

#define FOR_THE_FIELD(i, j, expression)\
for (int i = 0; i != 9; ++i) {\
    for (int j = 0; j != 9; ++j) {\
        expression\
    }\
}

#define FOR_ENEMY(i, j, expression)\
int c = i >> 1;\
for (int j = 2 - (c << 1); j != 4 - (c << 1); ++j) {\
    expression\
}


#endif //TANK_DEF_HPP
