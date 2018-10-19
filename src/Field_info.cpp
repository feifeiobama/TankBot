//
// Created by szc on 2018/10/17.
//

#include <cstring>
#include <iostream>
#include <iomanip>
#include <stack>
#include "Field_info.h"
#include "Field_map.h"

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
            refresh_distance(i, tank);
        }
    }

    // 计算 fire_map
    calc_fire_map(field_map);
}

Field_info::Field_info(const Field_info &field_info) {
    memcpy(clean_map, field_info.clean_map, 9 * sizeof(int));
}

void Field_info::update(Action action1, Action action2, const Field_map &field_map) {
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

    // 初始化 distance_map
    memset(distance_map, -1, 4 * 9 * 9 * sizeof(unsigned));
    for (int i = 0; i != 4; ++i) {
        Position tank = field_map.get_tank(i);
        if (tank != Null_pos) {
            refresh_distance(i, tank, field_map.get_loaded(i));
        }
    }
    // 计算 fire_map
    calc_fire_map(field_map);
}

void Field_info::calc_fire_map(const Field_map &field_map) {
    for (int i = 0; i != 4; ++i) {
        Position pos1 = field_map.get_tank(i);
        if (pos1 == Null_pos) {
            continue;
        }
        for (int j = 0; j != 4; ++j) {
            Position pos2 = get_adjacent_position(pos1, j);
            while (is_in_field(pos2)) {
                if (((clean_map[pos2.x] >> (pos2.y << 1)) & 0b11) != 0) {
                    if (pos2.x == 4 && ((pos2.y == 0 && i < 2) || (pos2.y == 8 && i >= 2))) {
                        int dir = (j + 2) % 4;
                        while(pos2 != pos1) {
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
}

void Field_info::refresh_distance(int i, Position position, bool loaded) {
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
int Field_info::dist_to_shoot_base(int tank, const Field_map &field_map) const {
    int base_row = 8 - ((tank / 2) << 3); // BLUE -> RED BASE
    unsigned base_row_barrier[9] = {0};
    unsigned last = 1;
    for (int i = 3; i >= 0; --i) {
        base_row_barrier[i] = last;
        if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
            last += 1;
        }
    }
    last = 1;
    for (int i = 5; i <= 8; ++i) {
        base_row_barrier[i] = last;
        if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
            last += 1;
        }
    }
    unsigned min_dist = unsigned(-1);
    for (int i = 0; i != 9; ++i) {
        if (i != 4) {
            // 不能连射
            unsigned dist = (2 * base_row_barrier[i] - 1) + distance_map[tank][i][base_row];
            if (min_dist > dist) {
                min_dist = dist;
            }
        }
    }
    return int(min_dist);
}

// 因为坦克机动性高，不允许隔着射击（容易被窄口挡住）(tank1 fire tank2)
int Field_info::dist_to_fire(int tank1, int tank2, const Field_map &field_map) const {
    unsigned min_dist = unsigned(-1);
    FOR_THE_FIELD(i, j, {
        if (fire_map[tank2][i][j]) {
            unsigned dist = distance_map[tank1][i][j];
            if (min_dist > dist) {
                min_dist = dist;
            }
        }
    })
    if (min_dist == unsigned(-1)) {
        Position pos = field_map.get_tank(tank2);
        min_dist = distance_map[tank1][pos.x][pos.y];
        return min_dist - 1;
    } else {
        if (min_dist == 0 && !field_map.get_loaded(tank1)) {
            min_dist += 1;
        }
        return min_dist + 1;
    }
}

// 追击坦克 or 己方协助
int Field_info::dist_to_tank(int tank1, int tank2, const Field_map &field_map) const {
    int distance = int(distance_map[tank1][field_map.get_tank(tank2).x][field_map.get_tank(tank2).y]);
    if (distance == 0) {
        distance = 2;
    }
    return distance;
}

// 视野宽敞 + 火力覆盖
int Field_info::area_fire(int tank, const Field_map &field_map) const {
    int area = 0;
    FOR_THE_FIELD(i, j, {
        if (fire_map[tank][i][j]) {
            area += 1;
        }
    })
    return area;
}

// 附近移动宽敞(应当避开敌方火线，和所有坦克坐标)
int Field_info::area_move(int tank, const Field_map &field_map) const {
    Position pos = field_map.get_tank(tank);
    Color color = Color(tank / 2);
    // 去掉所有坦克坐标
    int c0 = (color << 1) + 1 - tank, c1 = 2 - (color << 1), c2 = 3 - (color << 1);
    Position pos0 = field_map.get_tank(c0), pos1 = field_map.get_tank(c1), pos2 = field_map.get_tank(c2);
    int cnt = 0;
    FOR_ADJACENT_POS_INDEX(pos, adj_pos, {
        if (((clean_map[adj_pos.x] >> (adj_pos.y << 1)) & 0b11) != 0) {
            continue;
        }
        if (adj_pos == pos0 || adj_pos == pos1 || adj_pos == pos2) {
            continue;
        }
        if ((field_map.get_loaded(c1) && fire_map[c1][adj_pos.x][adj_pos.y]) ||
            (field_map.get_loaded(c2) && fire_map[c2][adj_pos.x][adj_pos.y])) {
            continue;
        }
        cnt += 1;
    })
    return cnt;
}

// 局部卡路线
bool Field_info::block_route(int tank1, int tank2, const Field_map &field_map) const {
    int dir = tank1 < 2 ? 0 : 2;
    Position pos1 = field_map.get_tank(tank1), pos2 = field_map.get_tank(tank2);
    Position pos3 = get_adjacent_position(pos2, dir);
    while(is_in_field(pos3)) {
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
    FOR_THE_FIELD(i ,j, {
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
    // dist to shoot base
    std::cout << "dist_to_shoot_base";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << ' ' << i << ':' << dist_to_shoot_base(i, field_map);
        }
    }
    std::cout << std::endl;
    // dist to fire
    std::cout << "dist_to_fire";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) != Null_pos) {
                    std::cout << ' ' << i << ',' << k << ':' << dist_to_fire(i, k, field_map);
                }
            }
        }
    }
    std::cout << std::endl;
    // dist to tank
    std::cout << "dist_to_tank";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            for (int j = 0; j != 4; ++j) {
                if (field_map.get_tank(j) != Null_pos) {
                    std::cout << ' ' << i << ',' << j << ':' << dist_to_tank(i, j, field_map);
                }
            }
        }
    }
    std::cout << std::endl;
    //area_fire
    std::cout << "area_fire";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << ' ' << i << ':' << area_fire(i, field_map);
        }
    }
    std::cout << std::endl;
    //area_move
    std::cout << "area_move";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            std::cout << ' ' << i << ':' << area_move(i, field_map);
        }
    }
    std::cout << std::endl;
#endif
}
