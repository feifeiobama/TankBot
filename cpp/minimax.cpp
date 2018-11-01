//
// Created by szc on 2018/10/16.
//

#ifndef TANK_DEF_HPP
#define TANK_DEF_HPP

#ifndef _BOTZONE_ONLINE
//#define _BOTZONE_ONLINE
#endif

#include <cstdint>
#include <limits.h>

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

//
// Created by szc on 2018/10/17.
//

#ifndef TANK_FIELD_MAP_H
#define TANK_FIELD_MAP_H


#include <vector>
#include <map>

using namespace std;

class Field_info;

// 当前状态的汇总, 位于MCT上每个节点, 置换表每个表项
class Field_map {
    int map[9]; // 9*(9*2) byte-0:是否有障碍物 byte-1:是可摧毁的 坦克位置在图中标为0b11
    Position tanks[4]; // 蓝坦克0 蓝坦克1 红坦克0 红坦克1 if dead [-1,-1]
    bool loaded[4];
    int round;  // 回合数
    Position destroyed_by[4]; // 缓存被毁的目标
public:
    Field_map(bool random_initialize);

    Field_map(const Field_map &field_map);

    bool is_empty(int i, int j) const;

    int block_type(int i, int j) const;

    void set_brick(int i, int j);

    void push_history(Action &action1, Action &action2, vector<pair<Field_map, Action> > history[2]) const;

    void update(Action &action1, Action &action2);

    int judge() const; // 0-蓝方赢 1-红方赢 -1-平局 2-继续

    bool is_avail(int i, Move m) const;

    int get_round() const;

    Position get_tank(int i) const;

    bool get_loaded(int i) const;

    Position get_destroyed_by(int i) const;

    int get_map(int i) const;

    void print() const;

    bool operator==(const Field_map &field_map) const;
};


#endif //TANK_FIELD_MAP_H

//
// Created by szc on 2018/10/17.
//

#ifndef TANK_FIELD_INFO_H
#define TANK_FIELD_INFO_H


#include <map>
#include <vector>

using namespace std;

class Field_map;

/*
 * 0 1 2 3 4 5 6 7 8 pos.x
 * 1 A B
 * 2 A B
 * 3 A B
 * 4 A B
 * 5 A B
 * 6 A B
 * 7 A B
 * 8 A B
 * pos.y
 */

// 各个坦克到周围距离信息的缓存, 位于MCT上每个节点，最好能够增量维护
class Field_info {
    int clean_map[9]; // 缓存的地图,坦克当作虚无,基地当作不能摧毁

    unsigned distance_map[4][9][9];
    bool fire_map[4][9][9] = {0};
    unsigned base_row_barrier[2][9];

public:
    Field_info(const Field_map &field_map);

    Field_info(const Field_info &field_info);

    void update(Action &action1, Action &action2, const Field_map &field_map);

    void calc_fire_map(int i, const Field_map &field_map);

    void calc_base_row_barrier();

    void calc_distance(int i, Position position, bool loaded = true);

    // 以下均保证坦克合法

    // 允许在其它位置射击击穿砖块
    unsigned dist_to_shoot_base(int tank, const Field_map &field_map, bool enemy=true) const;

    bool block_route(int tank1, int tank2, const Field_map &field_map) const;

    // <斩杀领先的步数, 最短的斩杀步数>
    pair<int, unsigned> dist_to_shoot_avoid(int tank1, int tank2, const Field_map &field_map) const;

    // 保证己方双坦都在 <完成骚操作操作需要的步数, 第一波威胁对面需要的步数>
    pair<unsigned, unsigned> dist_to_shoot_after(int tank, const Field_map &field_map) const;

    void print(const Field_map &field_map) const;
};


#endif //TANK_FIELD_INFO_H

//
// Created by szc on 2018/10/17.
//

#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <cstring>



Field_map::Field_map(bool random_initialize) {
    map[0] = map[1] = map[7] = map[8] = 0b000000001100000000;
    map[2] = map[6] = 0b110000001100000011;
    map[3] = map[5] = 0b111100001100001111;
    map[4] = 0b110111111111110111;

    tanks[0] = Position{2, 0};
    tanks[1] = Position{6, 0};
    tanks[2] = Position{6, 8};
    tanks[3] = Position{2, 8};

    for (int i = 0; i != 4; ++i) {
        loaded[i] = true;
    }
    round = 0;

    if (!random_initialize) {
        return;
    }

    srand48(clock());
    FOR_INIT_HALF_FIELD(i, j, {
        if (is_empty(i, j) && drand48() < 0.4) {
            set_brick(8 - i, 8 - j);
            set_brick(i, j);
        }
    })
}

Field_map::Field_map(const Field_map &field_map) {
    memcpy(map, field_map.map, 9 * sizeof(int));
    memcpy(tanks, field_map.tanks, 4 * sizeof(Position));
    memcpy(loaded, field_map.loaded, 4 * sizeof(bool));
    round = field_map.round;
}

bool Field_map::is_empty(int i, int j) const {
    return ((map[i] >> (j << 1)) & 0b11) == 0;
}

int Field_map::block_type(int i, int j) const {
    return (map[i] >> (j << 1)) & 0b11;
}

void Field_map::set_brick(int i, int j) {
    map[i] |= (0b11 << (j << 1));
}

void Field_map::push_history(Action &action1, Action &action2, vector<pair<Field_map, Action> > history[2]) const {
    history[0].push_back(make_pair(Field_map(*this), action1));
    history[1].push_back(make_pair(Field_map(*this), action2));
}

void Field_map::update(Action &action1, Action &action2) {
    round += 1;

    // 消除无意义的操作
    if (tanks[0] == Null_pos) {
        action1.move[0] = -1;
    }
    if (tanks[1] == Null_pos) {
        action1.move[1] = -1;
    }
    if (tanks[2] == Null_pos) {
        action2.move[0] = -1;
    }
    if (tanks[3] == Null_pos) {
        action2.move[1] = -1;
    }
    Move moves[4] = {action1.move[0], action1.move[1], action2.move[0], action2.move[1]};

    // 更新 loaded
    for (int i = 0; i != 4; ++i) {
        if (moves[i] >= 4) {
            loaded[i] = false;
        } else {
            loaded[i] = true;
        }
    }

    // 坦克向指定方向移动
    for (int i = 0; i != 4; ++i) {
        if (moves[i] <= 3 && moves[i] >= 0) {
            map[tanks[i].x] &= ~(0b11 << (tanks[i].y << 1));
            tanks[i].x += Dir[moves[i]][0];
            tanks[i].y += Dir[moves[i]][1];
            map[tanks[i].x] |= (0b11 << (tanks[i].y << 1));
        }
    }
    for (int i = 0; i != 4; ++i) {
        if (moves[i] == -1) {
            if (tanks[i] != Null_pos) {
                map[tanks[i].x] |= (0b11 << (tanks[i].y << 1));
            }
        }
    }

    // 发射炮弹 + 标记命中
    for (int i = 0; i != 4; ++i) {
        destroyed_by[i] = Null_pos;
    }
    bool to_destroy[4] = {false, false, false, false};
    for (int i = 0; i != 4; ++i) {
        if (moves[i] >= 4) {
            Position pos = tanks[i];
            pos = get_adjacent_position(pos, moves[i] - 4);
            while (is_in_field(pos)) {
                int type = block_type(pos.x, pos.y);
                if (type == 3) {
                    destroyed_by[i] = pos;
                    to_destroy[i] = true;
                    break;
                } else if (type == 1) {
                    break;
                }
                pos = get_adjacent_position(pos, moves[i] - 4);
            }
        }
    }

    // 剔除对射 + 去掉被摧毁的坦克的纪录
    int destroyed_time[4] = {0};
    int destroyed[4] = {0}; // destroyed[被摧毁] = 摧毁者
    for (int i = 0; i != 4; ++i) {
        if (to_destroy[i]) {
            for (int j = 0; j != 4; ++j) {
                if (destroyed_by[i] == tanks[j]) {
                    destroyed[j] = i;
                    destroyed_time[j] += 1;
                }
            }
        }
    }
    for (int i = 0; i != 4; ++i) {
        if (destroyed_time[i] == 1) {
            int destroyer = destroyed[i];
            if (destroyed_time[destroyer] == 1 && destroyed[destroyer] == i) {
                destroyed_time[destroyer] = 0; // 避免下次比较
                destroyed_by[i] = destroyed_by[destroyer] = Null_pos;
            } else {
                tanks[i] = Null_pos;
            }
        } else if (destroyed_time[i] > 1) {
            tanks[i] = Null_pos;
        }
    }
    // 地图上摧毁被标记的物体
    for (int i = 0; i != 4; ++i) {
        if (!(destroyed_by[i] == Null_pos)) {
            map[destroyed_by[i].x] &= ~(0b11 << (destroyed_by[i].y << 1));
        }
    }
}

// 0-蓝方赢 1-红方赢 -1-平局 2-继续
int Field_map::judge() const {
    int row = map[4];
    bool blue_base = (row & 1) != 0, red_base = ((row >> 16) & 1) != 0;
    bool blue_tank = tanks[0] != Null_pos || tanks[1] != Null_pos, red_tank =
            tanks[2] != Null_pos || tanks[3] != Null_pos;
    bool blue_dead = !(blue_base && blue_tank), red_dead = !(red_base && red_tank);
    int result = 2 - (red_dead << 1) - blue_dead;
    return round >= 100 && result == 2 ? -1 : result;
}

bool Field_map::is_avail(int tank, Move m) const {
    if (m == -1) {
        return true;
    } else {
        if (tanks[tank] == Null_pos) {
            return false;
        } else if (m >= 4) {
            return loaded[tank];
        } else {
            Position position = get_adjacent_position(tanks[tank], m);
            return is_in_field(position) && is_empty(position.x, position.y);
        }
    }
}

int Field_map::get_round() const {
    return round;
}

Position Field_map::get_tank(int i) const {
    return tanks[i];
}

bool Field_map::get_loaded(int i) const {
    return loaded[i];
}

Position Field_map::get_destroyed_by(int i) const {
    return destroyed_by[i];
}

int Field_map::get_map(int i) const {
    return map[i];
}

void Field_map::print() const {
#ifndef _BOTZONE_ONLINE
    char map[9][10] = {'\0'};
    bool multi[9][9] = {0};
    FOR_THE_FIELD(i, j, {
        map[j][i] = is_empty(i, j) ? ':' : '#';
    })
    char tank_char[4] = {'b', 'B', 'r', 'R'};
    for (int i = 0; i != 4; ++i) {
        if (tanks[i] != Null_pos) {
            map[tanks[i].y][tanks[i].x] = tank_char[i];
        }
    }
    for (int i = 0; i != 4; ++i) {
        if (tanks[i] == Null_pos) {
            continue;
        }
        if (multi[tanks[i].x][tanks[i].y]) {
            map[tanks[i].y][tanks[i].x] = '@';
        }
        multi[tanks[i].x][tanks[i].y] = true;
    }
    map[1][4] = map[7][4] = '%';
    map[0][4] = map[0][4] == '#' ? '*' : ':';
    map[8][4] = map[8][4] == '#' ? '*' : ':';
    for (int i = 0; i != 9; ++i) {
        std::cout << map[i] << std::endl;
    }
    for (int i = 0; i != 4; ++i) {
        std::cout << tank_char[i] << " pos " << tanks[i].x << ',' << tanks[i].y << " loaded " << loaded[i] << std::endl;
    }
#endif
}

bool Field_map::operator==(const Field_map &field_map) const {
    for (int i = 0; i != 4; ++i) {
        if (tanks[i] != field_map.tanks[i]) {
            return false;
        } else if (loaded[i] != field_map.loaded[i]) {
            return false;
        }
    }
    for (int i = 0; i != 9; ++i) {
        if (map[i] != field_map.map[i]) {
            return false;
        }
    }
    return true;
}

//
// Created by szc on 2018/10/17.
//

#include <cstring>
#include <iostream>
#include <iomanip>
#include <stack>



Field_info::Field_info(const Field_map &field_map) {
    for (int i = 0; i != 9; ++i) {
        clean_map[i] = field_map.get_map(i);
    }
    // 初始化 clean_map
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank != Null_pos) {
            clean_map[tank.x] &= ~(0b11 << (tank.y << 1));
        }
    }
    // 基地不可摧毁
    clean_map[4] &= ~0b10;
    clean_map[4] &= ~(0b10 << 16);

    // 初始化 distance_map
    memset(distance_map, -1, 4 * 9 * 9 * sizeof(unsigned));
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank != Null_pos) {
            calc_distance(i, tank);
        }
    }

    // 计算 fire_map
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank == Null_pos) {
            continue;
        }
        calc_fire_map(i, field_map);
    }

    // 计算 base_row_barrier
    calc_base_row_barrier();
}

Field_info::Field_info(const Field_info &field_info) {
    memcpy(clean_map, field_info.clean_map, 9 * sizeof(int));
    memcpy(fire_map, field_info.fire_map, 4 * 9 * 9 * sizeof(bool));
}

void Field_info::update(Action &action1, Action &action2, const Field_map &field_map) {
    // 修改 clean_map, 处理 destroyed
    Position destroyed[4];
    for (int i = 0; i != 4; ++i) {
        destroyed[i] = field_map.get_destroyed_by(i);
    }
    for (int i = 0; i != 4; ++i) {
        if (destroyed[i] != Null_pos) {
            if (((clean_map[destroyed[i].x] >> (destroyed[i].y << 1)) & 0b11) == 0) {
                //被毁的是坦克
                destroyed[i] = Null_pos;
            } else {
                clean_map[destroyed[i].x] &= ~(0b11 << (destroyed[i].y << 1));
            }
        }
    }

    Move m[4] = {action1.move[0], action1.move[1], action2.move[0], action2.move[1]};

    // 初始化 distance_map
    memset(distance_map, -1, 4 * 9 * 9 * sizeof(unsigned));
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank != Null_pos) {
            calc_distance(i, tank, field_map.get_loaded(i));
        }
    }

    // 计算 fire_map
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank == Null_pos) {
            continue;
        }
        if (m[i] >= 0 && m[i] < 4) {
            calc_fire_map(i, field_map);
        } else {
            for (int j = 0; j != 4; ++j) {
                if (tank.x == destroyed[j].x || tank.y == destroyed[j].y) {
                    fire_map[i][destroyed[j].x][destroyed[j].y] = true;
                }
            }
        }
    }

    // 计算 base_row_barrier
    calc_base_row_barrier();
}

void Field_info::calc_fire_map(int i, const Field_map &field_map) {
    Position pos1 = field_map.get_tank(i);
    for (int j = 0; j != 4; ++j) {
        Position pos2 = get_adjacent_position(pos1, j);
        while (is_in_field(pos2)) {
            if (((clean_map[pos2.x] >> (pos2.y << 1)) & 0b11) != 0) {
                if (pos2.x == 4 && ((pos2.y == 0 && i < 2) || (pos2.y == 8 && i >= 2))) {
                    int dir = (j + 2) % 4;
                    while (pos2 != pos1) {
                        fire_map[i][pos2.x][pos2.y] = false;
                        pos2 = get_adjacent_position(pos2, dir);
                    }
                }
                break;
            }
            fire_map[i][pos2.x][pos2.y] = true;
            pos2 = get_adjacent_position(pos2, j);
        }
    }
}

void Field_info::calc_base_row_barrier() {
    for (int c = 0; c != 2; ++c) {
        int base_row = c << 3; // BLUE -> BLUE BASE
        unsigned last = 1;
        for (int i = 3; i >= 0; --i) {
            base_row_barrier[c][i] = last;
            if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
                last += 1;
            }
        }
        last = 1;
        for (int i = 5; i <= 8; ++i) {
            base_row_barrier[c][i] = last;
            if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
                last += 1;
            }
        }
    }
}

void Field_info::calc_distance(int i, Position position, bool loaded) {
    std::stack<Position> searching, to_search;
    if (loaded) {
        distance_map[i][position.x][position.y] = 0;
        searching.push(position);
    } else {
        distance_map[i][position.x][position.y] = 0;
        FOR_ADJACENT_POS_INDEX(position, adj_pos, {
            int adj_state = (clean_map[adj_pos.x] >> (adj_pos.y << 1)) & 0b11;
            if (adj_state == 0b00) {
                distance_map[i][adj_pos.x][adj_pos.y] = 1;
                searching.push(adj_pos);
            } else if (adj_state == 0b11) {
                distance_map[i][adj_pos.x][adj_pos.y] = 3;
                searching.push(adj_pos);
            }

        })
    }

    while (!searching.empty()) {
        while (!searching.empty()) {
            Position curr_pos = searching.top();
            searching.pop();
            unsigned curr_distance = distance_map[i][curr_pos.x][curr_pos.y];
            FOR_ADJACENT_POS_INDEX(curr_pos, adj_pos, {
                int adj_state = (clean_map[adj_pos.x] >> (adj_pos.y << 1)) & 0b11;
                if (adj_state == 0b00) {
                    if (distance_map[i][adj_pos.x][adj_pos.y] > curr_distance + 1) {
                        distance_map[i][adj_pos.x][adj_pos.y] = curr_distance + 1;
                        to_search.push(adj_pos);
                    }
                } else if (adj_state == 0b11) {
                    if (distance_map[i][adj_pos.x][adj_pos.y] > curr_distance + 2) {
                        distance_map[i][adj_pos.x][adj_pos.y] = curr_distance + 2;
                        to_search.push(adj_pos);
                    }
                }
            })
        }
        searching = to_search;
        to_search = std::stack<Position>();
    }
}

// 允许在其它位置射击击穿砖块
unsigned Field_info::dist_to_shoot_base(int tank, const Field_map &field_map, bool enemy) const {
    int base_row = 8 - ((tank >> 1) << 3); // BLUE -> RED BASE
    int base_color = 1 - (tank >> 1);
    if (!enemy) {
        base_row = 8 - base_row;
        base_color = 1 - base_color;
    }
    unsigned min_dist = unsigned(-1);
    for (int i = 0; i != 9; ++i) {
        if (i != 4) {
            // 不能连射
            unsigned dist = (2 * base_row_barrier[base_color][i] - 1) + distance_map[tank][i][base_row];
            if (min_dist > dist) {
                min_dist = dist;
            }
        }
    }
    return min_dist;
}

// 局部卡路线
bool Field_info::block_route(int tank1, int tank2, const Field_map &field_map) const {
    int dir = tank1 < 2 ? 0 : 2;
    Position pos1 = field_map.get_tank(tank1), pos2 = field_map.get_tank(tank2);
    Position pos3 = get_adjacent_position(pos2, dir);
    while (is_in_field(pos3)) {
        if (pos3.y == 0 || pos3.y == 8) {
            break;
        }
        if (pos3 == pos1 || fire_map[tank1][pos3.x][pos3.y]) {
            return true;
        }
        pos3 = get_adjacent_position(pos3, dir);
    }
    return false;
}

// <斩杀领先的步数, 最短的斩杀步数>
pair<int, unsigned> Field_info::dist_to_shoot_avoid(int tank1, int tank2, const Field_map &field_map) const {
    int base_color = 1 - (tank1 >> 1);
    int base_row = base_color << 3; // BLUE -> RED BASE
    int next_row = 1 + base_color * 6;
    unsigned min_ahead_dist = unsigned(-1);
    int max_ahead = INT_MIN;

    for (int i = 0; i != 9; ++i) {
        if (i == 4) {
            continue;
        }

        unsigned dist = (2 * base_row_barrier[base_color][i] - 1) + distance_map[tank1][i][base_row];
        int min_ahead = INT_MAX;

        // 考虑在哪被拦截
        if (i < 4) {
            for (int j = i + 1; j != 4; ++j) {
                int e_dist = distance_map[tank2][j][next_row];
                if (min_ahead > e_dist + 2 - dist) {
                    min_ahead = e_dist + 2 - dist;
                }
            }
        } else {
            for (int j = 5; j != i; ++j) {
                int e_dist = distance_map[tank2][j][next_row];
                if (min_ahead > e_dist + 2 - dist) {
                    min_ahead = e_dist + 2 - dist;
                }
            }
        }

        int ahead = distance_map[tank2][i][base_row] + 1 - distance_map[tank1][i][base_row];
        if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
            ahead += 1;
        }
        if (min_ahead > ahead) {
            min_ahead = ahead;
        }

        // 考虑竖直方向位置造成的拦截关系
        if (block_route(tank2, tank1, field_map)) {
            min_ahead -= 1;
        }

        if (min_ahead > max_ahead) {
            max_ahead = min_ahead;
        }
        if (min_ahead_dist > dist) {
            min_ahead_dist = dist;
        }
    }

    return make_pair(max_ahead, min_ahead_dist);
}

// 保证己方双坦都在 <操作需要的步数, 第一波威胁对面需要的步数>
pair<unsigned, unsigned> Field_info::dist_to_shoot_after(int tank, const Field_map &field_map) const {
    int enemy_color = tank >> 1;
    int our_color = 1 - enemy_color;

    unsigned follow_dist = dist_to_shoot_base(tank, field_map, false);
    unsigned total_dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)},
            first_dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)};

    Position pos = field_map.get_tank(tank);

    for (int k = 0; k != 2; ++k) {
        for (int i = 0; i != 9; ++i) {
            if (fire_map[tank][i][pos.y]) {
                int tank2 = (our_color << 1) + k;
                unsigned dist = distance_map[tank2][i][pos.y];
                if (dist == 0 && !field_map.get_loaded(tank2)) {
                    dist += 1;
                }
                if (total_dist[k] > dist + abs(pos.x - i) + 1 + follow_dist) {
                    total_dist[k] = dist + abs(pos.x - i) + 1 + follow_dist;
                }
                if (first_dist[k] > dist + 1) {
                    first_dist[k] = dist + 1;
                }
            }
        }
        for (int i = 0; i != 9; ++i) {
            if (fire_map[tank][pos.x][i]) {
                int tank2 = (our_color << 1) + k;
                unsigned dist = distance_map[tank2][pos.x][i];
                if (dist == 0 && !field_map.get_loaded(tank2)) {
                    dist += 1;
                }
                if (total_dist[k + 2] > dist + abs(pos.y - i) + 1 + follow_dist) {
                    total_dist[k + 2] = dist + abs(pos.y - i) + 1 + follow_dist;
                }
                if (first_dist[k + 2] > dist + 1) {
                    first_dist[k + 2] = dist + 1;
                }
            }
        }
    }

    unsigned min_total_dist = min(max(total_dist[0], total_dist[3]), max(total_dist[1], total_dist[2])),
            min_first_dist = min(max(first_dist[0], first_dist[3]), max(first_dist[1], first_dist[2]));

    return make_pair(min_total_dist, min_first_dist);
}

void Field_info::print(const Field_map &field_map) const {
#ifndef _BOTZONE_ONLINE
    // 打印距离
    char tank_char[4] = {'b', 'B', 'r', 'R'};
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << tank_char[i] << std::endl;
            FOR_THE_FIELD(x, y, {
                std::cout << std::setw(3) << std::right << int(distance_map[i][y][x]);
                if (y == 8) {
                    std::cout << std::endl;
                }
            })
            std::cout << std::endl;
        }
    }
    // 打印 clean_map
    std::cout << "clean_map" << std::endl;
    FOR_THE_FIELD(i, j, {
        std::cout << std::setw(2) << std::right << ((clean_map[j] >> (i << 1)) & 0b11);
        if (j == 8) {
            std::cout << std::endl;
        }
    })
    // 打印 fire_map
    std::cout << "fire_map" << std::endl;
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << tank_char[i] << std::endl;
            FOR_THE_FIELD(x, y, {
                std::cout << std::setw(2) << std::right << int(fire_map[i][y][x]);
                if (y == 8) {
                    std::cout << std::endl;
                }
            })
            std::cout << std::endl;
        }
    }
    // 打印 base_row_barrier
    std::cout << "fire_map" << std::endl;
    for (int i = 0; i != 2; ++i) {
        for (int j = 0; j != 9; ++j) {
            cout << std::setw(2) << right << base_row_barrier[i][j];
        }
        cout << endl;
    }
    // dist to shoot base
    std::cout << "dist_to_shoot_base";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << ' ' << i << ':' << dist_to_shoot_base(i, field_map);
        }
    }
    std::cout << std::endl;
    // dist to shoot my base
    std::cout << "dist_to_shoot_my_base";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << ' ' << i << ':' << dist_to_shoot_base(i, field_map, false);
        }
    }
    std::cout << std::endl;
    // dist to shoot avoid
    std::cout << "dist_to_shoot_avoid";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) != Null_pos) {
                    auto p = dist_to_shoot_avoid(i, k, field_map);
                    std::cout << ' ' << i << ',' << k << ':' << p.first << ',' << p.second;
                }
            }
        }
    }
    std::cout << std::endl;
    // dist to shoot after
    std::cout << "dist_to_shoot_after";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            bool has_both = true;
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) != Null_pos) {
                    has_both = false;
                    break;
                }
            }
            if (!has_both) {
                continue;
            }
            auto p = dist_to_shoot_after(i, field_map);
            std::cout << ' ' << i << ':' << p.first << ',' << p.second;
        }
    }
    std::cout << std::endl;
#endif
}

//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_FIELD_HPP
#define PROJECT_FIELD_HPP

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>




constexpr int Argc = 10;
constexpr double Argv[Argc] = {1, 1, 2, 0.01, 0.01, 4, 1, 10, 0.2, 10};

class Field {
    Field_map field_map;
    Field_info field_info;
    int ending = -2;
public:
    Field(bool random_initialize=true) : field_map(random_initialize), field_info(field_map) {}

    Field(const Field_map &field_map1): field_map(field_map1), field_info(field_map1) {
        field_map.print();
        field_info.print(field_map);
        double* argv;
        evaluate(argv, true);
    }

    Field(const Field &field) : field_map(field.field_map), field_info(field.field_info) {}

    int update(Action action1, Action action2) {
        field_map.update(action1, action2);
        field_info.update(action1, action2, field_map);
        return field_map.judge();
    }

    Action find_history(vector<pair<Field_map, Action> > history) const {
        Action action = {-2, -2};
        for (auto p: history) {
            if (p.first == field_map) {
                return p.second;
            }
        }
        return action;
    }

    // 0-蓝方赢 1-红方赢 -1-平局 2-继续
    int judge() {
        if (ending != -2) {
            return ending;
        } else {
            ending = field_map.judge();
            return ending;
        }
    }

    // 返回蓝方胜率 (0,1) 禁止已经结束的局进入
    double evaluate(double argv[Argc], bool if_print=false) {
#ifdef _BOTZONE_ONLINE
        if_print = false;
#endif
        int pre_judge = judge();
        if (pre_judge == -1) {
            return 0.5;
        } else if (pre_judge == 0) {
            return 1.0;
        } else if (pre_judge == 1) {
            return 0.0;
        }
        bool if_tank_dead[4] = {false};
        for (int i = 0; i != 4; ++i) {
            if (field_map.get_tank(i) == Null_pos) {
                if_tank_dead[i] = true;
            }
        }

        // prepare values

        unsigned dist_to_shoot_base[4];
        pair<int, unsigned> dist_to_shoot_avoid[4][4];
        unsigned min_dist_to_shoot_avoid[4];
        pair<unsigned, unsigned> dist_to_shoot_after[4];

        for (int i = 0; i != 4; ++i) {
            min_dist_to_shoot_avoid[i] = unsigned(-1);
            if (if_tank_dead[i]) {
                continue;
            }
            dist_to_shoot_base[i] = field_info.dist_to_shoot_base(i, field_map);
            bool both_has = true;
            FOR_ENEMY(i, j, {
                if (if_tank_dead[j]) {
                    both_has = false;
                    continue;
                }
                dist_to_shoot_avoid[i][j] = field_info.dist_to_shoot_avoid(i, j, field_map);
                if (min_dist_to_shoot_avoid[i] > dist_to_shoot_avoid[i][j].second) {
                    min_dist_to_shoot_avoid[i] = dist_to_shoot_avoid[i][j].second;
                }
            })
            if (both_has) {
                dist_to_shoot_after[i] = field_info.dist_to_shoot_after(i, field_map);
            }
        }

        // calc basic values

        double blue = 0, red = 0;
        blue -= 0.1 * (min_dist_to_shoot_avoid[0] + min_dist_to_shoot_avoid[1]);
        red -= 0.1 * (min_dist_to_shoot_avoid[2] + min_dist_to_shoot_avoid[3]);

        if (if_print) {
            std::cout << blue << " " << red << endl;
        }

        return double(1) / (1 + exp(red - blue));
    }

    bool is_avail(int tank, Move m) const {
        return field_map.is_avail(tank, m);
    }

    int get_round() {
        return field_map.get_round();
    }

    void print() {
        field_map.print();
        field_info.print(field_map);
#ifndef _BOTZONE_ONLINE
        double argv[Argc];
        memcpy(argv, Argv, Argc * sizeof(double));
        evaluate(argv, true);
#endif
    }
};

#endif //PROJECT_FIELD_HPP

//
// Created by szc on 2018/10/16.
//

#ifndef TANK_IO_HANDLER_HPP
#define TANK_IO_HANDLER_HPP

#include <iostream>
#include "jsoncpp/json.h"



Color recover_from_input(Field_map &field_map, vector<pair<Field_map, Action> > history[2]) {
    Json::Reader reader;
    Json::Value input;
    std::string inputString;
    do {
        getline(std::cin, inputString);
    } while (inputString.empty());
#ifndef _BOTZONE_ONLINE
    // 猜测是单行还是多行
    char lastChar = inputString[inputString.size() - 1];
    if (lastChar != '}' && lastChar != ']') {
        // 第一行不以}或]结尾，猜测是多行
        std::string newString;
        do {
            getline(std::cin, newString);
            inputString += newString;
        } while (newString != "}" && newString != "]");
    }
#endif
    reader.parse(inputString, input);

    Json::Value requests = input["requests"], responses = input["responses"];

    // 处理砖块
    Json::Value bricks = requests[0]["field"];
    int has_brick[3];
    for (int i = 0; i != 3; ++i) {
        has_brick[i] = bricks[i].asInt();
    }
    FOR_THE_FIELD(i, j, {
        int index = i + 9 * j;
        if ((has_brick[index / 27] >> (index % 27)) & 1) {
            field_map.set_brick(i, j);
        }
    })

    // 己方颜色
    Color color = Color(requests[0]["mySide"].asInt());

    // 处理坦克动作
    size_t n = responses.size();
    for (int i = 0; i < n; i++) {
        Json::Value response = responses[i], request = requests[i + 1];
        Action action[2];
        action[color] = Action{Move(response[0].asInt()), Move(response[1].asInt())};
        action[op_color(color)] = Action{Move(request[0].asInt()), Move(request[1].asInt())};
        field_map.push_history(action[0], action[1], history);
        field_map.update(action[0], action[1]);
    }

    //打印一波
    return color;
}

void encode_output(Action action, std::string debug = "") {
#ifdef _BOTZONE_ONLINE
    Json::FastWriter writer;
#else
    Json::StyledWriter writer;
#endif
    Json::Value output(Json::objectValue), response(Json::arrayValue);
    for (int i = 0; i != 2; ++i) {
        response[i] = action.move[i];
    }
    output["response"] = response;
    if (!debug.empty()) {
        output["debug"] = debug;
    }
    std::cout << writer.write(output) << std::endl;
}

#endif //TANK_IO_HANDLER_HPP

//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_GREEDY_HPP
#define PROJECT_GREEDY_HPP

#include <cstring>


class Minimax_players {
    double argv[20];
public:
    Minimax_players() {
        memcpy(argv, Argv, Argc * sizeof(double));
    }

    Minimax_players(double argv1[]) {
        memcpy(argv, argv1, Argc * sizeof(double));
    }

    double get_val(Move m1, Move m2, Move m3, Move m4, const Field &field, bool if_print = false) {
        Field field1 = Field(field);
        field1.update(Action{m1, m2}, Action{m3, m4});
        if (if_print) {
            field1.print();
        }
        return field1.evaluate(argv);
    }

    Action make_decision(Color color, const Field &field, vector<pair<Field_map, Action> > history[2]) {
        // 判定双方可做的动作 (不管怎样 -1 应该都是可以的)
        bool is_avail[4][9];
        for (int i = 0; i != 4; ++i) {
            for (int m = -1; m != 8; ++m) {
                is_avail[i][m + 1] = field.is_avail(i, m);
            }
        }

        Action a = field.find_history(history[1 - color]);

        // 带剪枝的 minimax
        Action ans = {-1, -1};
        if (color == BLUE) {
            double max_val = 0.0;
            for (int m1 = -1; m1 != 8; ++m1) {
                if (!is_avail[0][m1 + 1]) {
                    continue;
                }
                for (int m2 = -1; m2 != 8; ++m2) {
                    if (!is_avail[1][m2 + 1]) {
                        continue;
                    }
                    double min_val = 1.0;
                    int m3, m4;

                    if (a.move[0] != -2) {
                        m3 = a.move[0];
                        m4 = a.move[1];
                        double val = get_val(m1, m2, m3, m4, field);
                        if (val < min_val) {
                            min_val = val;
                        }
                    } else {
                        for (m3 = -1; m3 != 8; ++m3) {
                            if (!is_avail[2][m3 + 1]) {
                                continue;
                            }
                            for (m4 = -1; m4 != 8; ++m4) {
                                if (!is_avail[3][m4 + 1]) {
                                    continue;
                                }
                                double val = get_val(m1, m2, m3, m4, field);
                                if (val < max_val) {
                                    goto label1;
                                }
                                if (val < min_val) {
                                    min_val = val;
                                }
                            }
                        }
                    }

                    if (min_val > max_val) {
                        max_val = min_val;
                        ans = Action{m1, m2};
                    }
                    label1:;
                }
            }
        } else {
            double min_val = 1.0;
            for (int m3 = -1; m3 != 8; ++m3) {
                if (!is_avail[2][m3 + 1]) {
                    continue;
                }
                for (int m4 = -1; m4 != 8; ++m4) {
                    if (!is_avail[3][m4 + 1]) {
                        continue;
                    }
                    double max_val = 0.0;
                    int m1, m2;

                    if (a.move[0] != -2) {
                        m1 = a.move[0];
                        m2 = a.move[1];
                        double val = get_val(m1, m2, m3, m4, field);
                        if (val > max_val) {
                            max_val = val;
                        }
                    } else {
                        for (int m1 = -1; m1 != 8; ++m1) {
                            if (!is_avail[0][m1 + 1]) {
                                continue;
                            }
                            for (int m2 = -1; m2 != 8; ++m2) {
                                if (!is_avail[1][m2 + 1]) {
                                    continue;
                                }
                                double val = get_val(m1, m2, m3, m4, field);
                                if (val > min_val) {
                                    goto label2;
                                }
                                if (val > max_val) {
                                    max_val = val;
                                }
                            }
                        }
                    }
                    if (max_val < min_val) {
                        min_val = max_val;
                        ans = Action{m3, m4};
                    }
                    label2:;
                }
            }
        }

        return ans;
    }
};

#endif //PROJECT_GREEDY_HPP

//
// Created by szc on 2018/10/16.
//

#ifndef TANK_GAME_HPP
#define TANK_GAME_HPP

extern "C" {
void game_start();
};

#endif //TANK_GAME_HPP

//
// Created by szc on 2018/10/16.
//





#include <iostream>

extern "C" {
void game_start() {
    std::ios::sync_with_stdio(false);

    vector<pair<Field_map, Action> > history[2];
    Field_map field_map = Field_map(false);
    Color currentColor = recover_from_input(field_map, history);

    Field field = Field(field_map);
    Minimax_players player = Minimax_players();
    Action action = player.make_decision(currentColor, field, history);
    encode_output(action);
}
}

int main() {
    game_start();
    return 0;
}