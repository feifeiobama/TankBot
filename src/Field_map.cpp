//
// Created by szc on 2018/10/17.
//

#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <cstring>
#include "Field_map.h"
#include "Field_info.h"

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
    return ((map[i] >> (j << 1)) & 0b01) == 0;
}

int Field_map::block_type(int i, int j) const {
    return (map[i] >> (j << 1)) & 0b11;
}

void Field_map::set_brick(int i, int j) {
    map[i] |= (0b11 << (j << 1));
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
    if (round != field_map.round) {
        return false;
    }
    for (int i = 0; i != 4; ++i) {
        if (tanks[i] != field_map.tanks[i]) {
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
