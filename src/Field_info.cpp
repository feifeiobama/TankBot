//
// Created by szc on 2018/10/17.
//

#include <cstring>
#include <iostream>
#include <iomanip>
#include <stack>
#include "Field_info.h"
#include "Field_map.h"

using namespace std;

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
            calc_distance(i, tank, field_map.get_loaded(i));
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
            memset(fire_map[i], 0, 9 * 9 * sizeof(bool));
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
    Position s[3][81];
    int index[3][2] = {0};
    unsigned step;
    bool searched[9][9] = {0};

    if (loaded) {
        distance_map[i][position.x][position.y] = 0;
        s[0][0] = position;
        index[0][1] = 1;
        step = 0;
    } else {
        distance_map[i][position.x][position.y] = 0;
        FOR_ADJACENT_POS_INDEX(position, adj_pos, {
            int adj_state = (clean_map[adj_pos.x] >> (adj_pos.y << 1)) & 0b11;
            if (adj_state == 0b00) {
                distance_map[i][adj_pos.x][adj_pos.y] = 1;
                s[1][index[1][1]] = adj_pos;
                index[1][1] = index[1][1] + 1;
            } else if (adj_state == 0b11) {
                distance_map[i][adj_pos.x][adj_pos.y] = 3;
                s[0][index[0][1]] = adj_pos;
                index[0][1] = index[0][1] + 1;
            }

        })
        step = 1;
    }

    while (index[0][0] != index[0][1] || index[1][0] != index[1][1] || index[2][0] != index[2][1]) {
        while (index[step % 3][0] != index[step % 3][1]) {
            Position curr_pos = s[step % 3][index[step % 3][0]];
            index[step % 3][0] = index[step % 3][0] + 1;
            if (searched[curr_pos.x][curr_pos.y]) {
                continue;
            } else {
                searched[curr_pos.x][curr_pos.y] = true;
            }
            FOR_ADJACENT_POS_INDEX(curr_pos, adj_pos, {
                int adj_state = (clean_map[adj_pos.x] >> (adj_pos.y << 1)) & 0b11;
                if (adj_state == 0b00) {
                    if (distance_map[i][adj_pos.x][adj_pos.y] < step + 1) {
                        continue;
                    }
                    distance_map[i][adj_pos.x][adj_pos.y] = step + 1;
                    s[(step + 1) % 3][index[(step + 1) % 3][1]] = adj_pos;
                    index[(step + 1) % 3][1] = index[(step + 1) % 3][1] + 1;
                    continue;
                } else if (adj_state == 0b11) {
                    if (distance_map[i][adj_pos.x][adj_pos.y] < step + 2) {
                        continue;
                    }
                    distance_map[i][adj_pos.x][adj_pos.y] = step + 2;
                    s[(step + 2) % 3][index[(step + 2) % 3][1]] = adj_pos;
                    index[(step + 2) % 3][1] = index[(step + 2) % 3][1] + 1;
                }
            })
        }
        step += 1;
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
            if (distance_map[tank][i][base_row] == 0 && field_map.get_loaded(tank)) {
                dist -= 1;
            }

            if (min_dist > dist) {
                min_dist = dist;
            }
        }
    }
    return min_dist;
}

// <被延缓的步数，被禁止通过>
pair<int, bool> Field_info::block_route(int tank1, int tank2, const Field_map &field_map) const {
    Position pos1 = field_map.get_tank(tank1), pos2 = field_map.get_tank(tank2);
    int counter = 0;

    // 计算拖延的步数(0/1)
    if (pos1 != pos2) {
        // y方向挡住 && x方向挡住
        if ((tank2 < 2 && pos1.y > pos2.y) || (tank2 >= 2 && pos1.y < pos2.y)) {
            int base_row = (tank1 >> 1) << 3;

            if (pos1.x == pos2.x) {
                if (pos1.y != base_row) {
                    counter = 1;
                }
            } else if (fire_map[tank1][pos2.x][pos1.y]) {
                if (abs(pos1.y - pos2.y) != 1 || field_map.get_loaded(tank1)) {
                    if ((pos2.x - pos1.x) * (pos2.x - 4) >= 0) {
                        counter = 1;
                    } else if (pos1.y != base_row) {
                        counter = 1;
                    }
                }
            }
        }
    }
    if (pos1 == pos2 || (pos1.x - 4) * (pos2.x - 4) <= 0 || abs(pos1.x - 4) > abs(pos2.x - 4)) {
        return make_pair(counter, false);
    }
    if (tank2 < 2) {
        if (pos1.y < pos2.y) {
            return make_pair(counter, false);
        }
        int max_j = 0;
        for (int j = 8; j >= 0; --j) {
            if (fire_map[tank2][pos2.x][j]) {
                max_j = j;
                break;
            }
        }
        max_j = max(max_j, pos2.y);
        if (pos1.y < max_j && !fire_map[tank1][pos1.x][max_j]) {
            return make_pair(counter, false);
        }
    } else {
        if (pos1.y > pos2.y) {
            return make_pair(counter, false);
        }
        int min_j = 8;
        for (int j = 0; j != 9; ++j) {
            if (fire_map[tank2][pos2.x][j]) {
                min_j = j;
                break;
            }
        }
        min_j = min(min_j, pos2.y);
        if (pos1.y > min_j && !fire_map[tank1][pos1.x][min_j]) {
            return make_pair(counter, false);
        }
    }
    if (pos2.x < 4) {
        int max_i = 0;
        for (int i = 3; i >= 0; --i) {
            if (fire_map[tank2][i][pos2.y]) {
                max_i = i;
                break;
            }
        }
        max_i = max(max_i, pos2.x);
        if (pos1.x < max_i && !fire_map[tank1][max_i][pos1.y]) {
            return make_pair(counter, false);
        } else {
            return make_pair(counter, true);
        }
    } else {
        int min_i = 8;
        for (int i = 5; i != 9; ++i) {
            if (fire_map[tank2][i][pos2.y]) {
                min_i = i;
                break;
            }
        }
        min_i = min(min_i, pos2.x);
        if (pos1.x > min_i && !fire_map[tank1][min_i][pos1.y]) {
            return make_pair(counter, false);
        } else {
            return make_pair(counter, true);
        }
    }
}

pair<int, bool> Field_info::blocked_route(int tank, const Field_map &field_map) const {
    Position pos = field_map.get_tank(tank);
    int tank1 = 2 - ((tank >> 1) << 1), tank2 = 3 - ((tank >> 1) << 1);
    Position pos1 = field_map.get_tank(tank1), pos2 = field_map.get_tank(tank2);

    auto p1 = block_route(tank1, tank, field_map), p2 = block_route(tank2, tank, field_map);
    int counter = p1.first + p2.first;
    if (pos1.y == pos2.y && p1.first != 0 && p2.first != 0) {
        counter -= 1;
    }

    if (pos.x == 4) {
        if ((pos1.x != 3 || pos2.x != 5) && (pos1.x != 5 || pos2.x != 3)) {
            return make_pair(counter, false);
        }
        if (tank < 2) {
            if (pos.y > pos1.y || pos.y > pos2.y) {
                return make_pair(counter, false);
            }
            int max_j = 0;
            for (int j = 8; j >= 0; --j) {
                if (fire_map[tank][4][j]) {
                    max_j = j;
                    break;
                }
            }
            max_j = max(max_j, pos.y);
            if (pos1.y < max_j && !fire_map[tank1][pos1.x][max_j]) {
                return make_pair(counter, false);
            } else if (pos2.y < max_j && !fire_map[tank2][pos2.x][max_j]) {
                return make_pair(counter, false);
            } else {
                return make_pair(counter, true);
            }
        } else {
            if (pos.y < pos1.y || pos.y < pos2.y) {
                return make_pair(counter, false);
            }
            int min_j = 8;
            for (int j = 0; j != 9; ++j) {
                if (fire_map[tank][4][j]) {
                    min_j = j;
                    break;
                }
            }
            min_j = min(min_j, pos.y);
            if (pos1.y > min_j && !fire_map[tank1][pos1.x][min_j]) {
                return make_pair(counter, false);
            } else if (pos2.y > min_j && !fire_map[tank2][pos2.x][min_j]) {
                return make_pair(counter, false);
            } else {
                return make_pair(counter, true);
            }
        }
    } else {
        return make_pair(counter, p1.second || p2.second);
    }
}

// <斩杀领先的步数, 最短的斩杀步数（-1）>
pair<int, unsigned> Field_info::dist_to_shoot_avoid(int tank1, int tank2, const Field_map &field_map) const {
    int base_color = 1 - (tank1 >> 1);
    int base_row = base_color << 3; // BLUE -> RED BASE
    int next_row = 1 + base_color * 6;
    unsigned min_ahead_dist = unsigned(-1);
    int max_ahead = INT_MIN;

    auto blk_route = block_route(tank2, tank1, field_map);

    unsigned e_dist[9] = {0};
    int min_e_dist = INT_MAX;
    for (int i = 3; i >= 0; --i) {
        e_dist[i] = min_e_dist;
        int e_dist = min(distance_map[tank2][i][next_row] + 1, distance_map[tank2][i][base_row]);
        if (distance_map[tank2][i][base_row] == 0 && !field_map.get_loaded(tank2)) {
            e_dist += 1;
        }
        if (min_e_dist > e_dist) {
            min_e_dist = e_dist;
        }
    }
    min_e_dist = INT_MAX;
    for (int i = 5; i != 9; ++i) {
        e_dist[i] = min_e_dist;
        int e_dist = min(distance_map[tank2][i][next_row] + 1, distance_map[tank2][i][base_row]);
        if (distance_map[tank2][i][base_row] == 0 && !field_map.get_loaded(tank2)) {
            e_dist += 1;
        }
        if (min_e_dist > e_dist) {
            min_e_dist = e_dist;
        }
    }

    for (int i = 0; i != 9; ++i) {
        if (i == 4) {
            continue;
        }

        unsigned dist = (2 * base_row_barrier[base_color][i] - 1) + distance_map[tank1][i][base_row];
        if (distance_map[tank1][i][base_row] == 0 && !field_map.get_loaded(tank1)) {
            dist += 1;
        }

        // 考虑在哪被拦截
        int min_ahead = e_dist[i] - dist;

        int ahead = distance_map[tank2][i][base_row] - distance_map[tank1][i][base_row];

        if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
            ahead += 1;
        }
        if (min_ahead > ahead) {
            min_ahead = ahead;
        }

        // 考虑拦截关系
        if (blk_route.first == 1) {
            if (blk_route.second || drand48() < 0.5) {
                min_ahead -= blk_route.first;
            }
            if (blk_route.second || drand48() < 0.5) {
                dist += blk_route.first;
            }
        }

        if (min_ahead > max_ahead) {
            max_ahead = min_ahead;
        }

        if (min_ahead >= 0 && min_ahead_dist > dist) {
            min_ahead_dist = dist;
        }
    }

    return make_pair(max_ahead, min_ahead_dist);
}

// 保证对面有两个，返回 <斩杀领先的步数>=0, 最短的斩杀步数>
pair<int, unsigned> Field_info::dist_to_shoot_avoid_both(int tank, const Field_map &field_map) const {
    int base_color = 1 - (tank >> 1);
    int tank1 = (base_color << 1), tank2 = (base_color << 1) + 1;
    int base_row = base_color << 3; // BLUE -> RED BASE
    int next_row = 1 + base_color * 6;
    unsigned min_ahead_dist = unsigned(-1);
    int max_ahead = INT_MIN;

    auto blk_route = blocked_route(tank, field_map);

    int e_dist[9] = {0};
    int min_e_dist = INT_MAX;
    for (int i = 3; i > 0; --i) {
        e_dist[i] = min_e_dist;
        int e_dist = min(min(distance_map[tank1][i][next_row] + 1, distance_map[tank1][i][base_row]),
                         min(distance_map[tank2][i][next_row] + 1, distance_map[tank2][i][base_row]));
        if ((distance_map[tank1][i][base_row] == 0 && !field_map.get_loaded(tank1)) &&
            (distance_map[tank2][i][base_row] == 0 && !field_map.get_loaded(tank2))) {
            e_dist += 1;
        }
        if (min_e_dist > e_dist) {
            min_e_dist = e_dist;
        }
    }
    min_e_dist = INT_MAX;
    for (int i = 5; i != 9; ++i) {
        e_dist[i] = min_e_dist;
        int e_dist = min(min(distance_map[tank1][i][next_row] + 1, distance_map[tank1][i][base_row]),
                         min(distance_map[tank2][i][next_row] + 1, distance_map[tank2][i][base_row]));
        if ((distance_map[tank1][i][base_row] == 0 && !field_map.get_loaded(tank1)) &&
            (distance_map[tank2][i][base_row] == 0 && !field_map.get_loaded(tank2))) {
            e_dist += 1;
        }
        if (min_e_dist > e_dist) {
            min_e_dist = e_dist;
        }
    }

    for (int i = 0; i != 9; ++i) {
        if (i == 4) {
            continue;
        }

        unsigned dist = (2 * base_row_barrier[base_color][i] - 1) + distance_map[tank][i][base_row];
        if (distance_map[tank][i][base_row] == 0 && !field_map.get_loaded(tank)) {
            dist += 1;
        }

        // 考虑在哪被拦截
        int min_ahead = e_dist[i] - dist;

        int ahead = min(distance_map[tank1][i][base_row], distance_map[tank2][i][base_row]) -
                    distance_map[tank][i][base_row];

        if (((clean_map[i] >> (base_row << 1)) & 0b11) != 0) {
            ahead += 1;
        }
        if (min_ahead > ahead) {
            min_ahead = ahead;
        }

        // 考虑拦截关系
        for (int j = 0; j != blk_route.first; ++j) {
            if (blk_route.second || drand48() < 0.5) {
                min_ahead -= blk_route.first;
            }
            if (blk_route.second || drand48() < 0.5) {
                dist += blk_route.first;
            }
        }

        if (min_ahead > max_ahead) {
            max_ahead = min_ahead;
        }
        if (min_ahead >= 0 && min_ahead_dist > dist) {
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
    unsigned move_dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)},
            first_dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)};

    Position pos = field_map.get_tank(tank);

    for (int k = 0; k != 2; ++k) {
        for (int i = 0; i != 9; ++i) {
            if (fire_map[tank][i][pos.y] || (abs(i - pos.x) == 1 && ((clean_map[i] >> (pos.y << 1)) & 0b11) == 0b11)) {
                int tank2 = (our_color << 1) + k;
                unsigned dist = distance_map[tank2][i][pos.y];
                if (dist == 0 && !field_map.get_loaded(tank2)) {
                    dist += 1;
                }
                if (move_dist[k] > dist + abs(pos.x - i)) {
                    move_dist[k] = dist + abs(pos.x - i);
                }
                if (first_dist[k] > dist + 1) {
                    first_dist[k] = dist + 1;
                }
            }
        }
        for (int i = 0; i != 9; ++i) {
            if (fire_map[tank][pos.x][i] || (abs(i - pos.y) == 1 && ((clean_map[pos.x] >> (i << 1)) & 0b11) == 0b11)) {
                int tank2 = (our_color << 1) + k;
                unsigned dist = distance_map[tank2][pos.x][i];
                if (dist == 0 && !field_map.get_loaded(tank2)) {
                    dist += 1;
                }
                if (move_dist[k + 2] > dist + abs(pos.y - i)) {
                    move_dist[k + 2] = dist + abs(pos.y - i);
                }
                if (first_dist[k + 2] > dist + 1) {
                    first_dist[k + 2] = dist + 1;
                }
            }
        }
    }

    unsigned min_total_dist =
            min(min(max(move_dist[0], first_dist[3]), max(first_dist[0], move_dist[3])),
                min(max(move_dist[1], first_dist[2]), max(first_dist[1], move_dist[2]))) + follow_dist + 1,
            min_first_dist = min(max(first_dist[0], first_dist[3]), max(first_dist[1], first_dist[2]));

    return make_pair(min_total_dist, min_first_dist);
}

// 视野宽敞 + 火力覆盖
unsigned Field_info::area_fire(int tank, const Field_map &field_map) const {
    unsigned area = 0;
    FOR_THE_FIELD(i, j, {
        if (fire_map[tank][i][j]) {
            area += 1;
        }
    })
    return area;
}

// 附近移动宽敞(应当避开敌方火线，和所有坦克坐标)
unsigned Field_info::area_move(int tank, const Field_map &field_map) const {
    Position pos = field_map.get_tank(tank);
    Color color = Color(tank / 2);
    // 去掉所有坦克坐标
    int c0 = (color << 1) + 1 - tank, c1 = 2 - (color << 1), c2 = 3 - (color << 1);
    Position pos0 = field_map.get_tank(c0), pos1 = field_map.get_tank(c1), pos2 = field_map.get_tank(c2);
    unsigned cnt = 0;
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

void Field_info::print(const Field_map &field_map) const {
#ifndef _BOTZONE_ONLINE
    // 打印距离
    char tank_char[4] = {'b', 'B', 'r', 'R'};
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << tank_char[i] << endl;
            FOR_THE_FIELD(x, y, {
                cout << setw(3) << right << int(distance_map[i][y][x]);
                if (y == 8) {
                    cout << endl;
                }
            })
            cout << endl;
        }
    }
    // 打印 clean_map
    cout << "clean_map" << endl;
    FOR_THE_FIELD(i, j, {
        cout << setw(2) << right << ((clean_map[j] >> (i << 1)) & 0b11);
        if (j == 8) {
            cout << endl;
        }
    })
    // 打印 fire_map
    cout << "fire_map" << endl;
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << tank_char[i] << endl;
            FOR_THE_FIELD(x, y, {
                cout << setw(2) << right << int(fire_map[i][y][x]);
                if (y == 8) {
                    cout << endl;
                }
            })
            cout << endl;
        }
    }
    // 打印 base_row_barrier
    cout << "row_barrier" << endl;
    for (int i = 0; i != 2; ++i) {
        for (int j = 0; j != 9; ++j) {
            cout << setw(2) << right << base_row_barrier[i][j];
        }
        cout << endl;
    }
    cout << endl;
    // dist to shoot base
    cout << "dist_to_shoot_base";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << ' ' << i << ':' << dist_to_shoot_base(i, field_map);
        }
    }
    cout << endl;
    // dist to shoot my base
    cout << "dist_to_shoot_my_base";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << ' ' << i << ':' << dist_to_shoot_base(i, field_map, false);
        }
    }
    cout << endl;
    // block route
    cout << "block_route";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) != Null_pos) {
                    auto p = block_route(i, k, field_map);
                    cout << ' ' << i << ',' << k << ':' << p.first << ',' << p.second;
                }
            }
        }
    }
    cout << endl;
    // blocked route
    cout << "blocked_route";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            bool has_both = true;
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) == Null_pos) {
                    has_both = false;
                    break;
                }
            }
            if (!has_both) {
                continue;
            }
            auto p = blocked_route(i, field_map);
            cout << ' ' << i << ":" << p.first << "," << p.second;
        }
    }
    cout << endl;
    // dist to shoot avoid
    cout << "dist_to_shoot_avoid";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) != Null_pos) {
                    auto p = dist_to_shoot_avoid(i, k, field_map);
                    cout << ' ' << i << ',' << k << ':' << p.first << ',' << int(p.second);
                }
            }
        }
    }
    cout << endl;
    // dist to shoot avoid
    cout << "dist_to_shoot_avoid_both";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            bool has_both = true;
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) == Null_pos) {
                    has_both = false;
                    break;
                }
            }
            if (!has_both) {
                continue;
            }
            auto p = dist_to_shoot_avoid_both(i, field_map);
            cout << ' ' << i << ":" << p.first << "," << int(p.second);
        }
    }
    cout << endl;
    // dist to shoot after
    cout << "dist_to_shoot_after";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            bool has_both = true;
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                if (field_map.get_tank(k) == Null_pos) {
                    has_both = false;
                    break;
                }
            }
            if (!has_both) {
                continue;
            }
            auto p = dist_to_shoot_after(i, field_map);
            cout << ' ' << i << ':' << int(p.first) << ',' << int(p.second);
        }
    }
    cout << endl;
    //area_fire
    cout << "area_fire";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << ' ' << i << ':' << area_fire(i, field_map);
        }
    }
    cout << endl;
    //area_move
    cout << "area_move";
    for (int i = 0; i != 4; ++i) {
        if (field_map.get_tank(i) != Null_pos) {
            cout << ' ' << i << ':' << area_move(i, field_map);
        }
    }
    cout << endl;
#endif
}
