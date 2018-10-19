//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_FIELD_HPP
#define PROJECT_FIELD_HPP

#include <cmath>
#include <cstring>
#include <iostream>
#include "def.hpp"
#include "Field_map.h"
#include "Field_info.h"

constexpr int Argc = 10;
constexpr double Argv[Argc] = {1, 1, 2, 0.01, 0.01, 4, 1, 10, 0.2, 10};

class Field {
    Field_map field_map;
    Field_info field_info;
    int ending = -2;
public:
    Field() : field_map(true), field_info(field_map) {}

    Field(const Field_map &field_map1) : field_map(field_map1), field_info(field_map1) {
        print();
    }

    Field(const Field &field) : field_map(field.field_map), field_info(field.field_info) {}

    int update(Action action1, Action action2) {
        field_map.update(action1, action2);
        field_info.update(action1, action2, field_map);
        return field_map.judge();
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
        // calc paired values
        double single_threat[4][4] = {0}, single_counter[4][4] = {0};
        for (int i = 0; i != 4; ++i) {
            if (!if_tank_dead[i]) {
                Color c1 = Color(i / 2), c2 = Color(1 - c1);
                for (int j = 0; j != 2; ++j) {
                    int k = (c2 << 1) + j;
                    if (!if_tank_dead[k]) {
                        single_threat[i][k] =
                                double(1) / field_info.dist_to_tank(i, k, field_map) +
                                argv[0] / field_info.dist_to_fire(i, k, field_map);
                        single_counter[i][k] =
                                double(1) / field_info.dist_to_fire(i, k, field_map) +
                                argv[1] / field_info.dist_to_tank(i, k, field_map);
                        if (field_info.block_route(i, k, field_map)) {
                            single_counter[i][k] += argv[2];
                        }
                    }
                }
            }
        }

        // calc single tank values
        double single_invade[4], single_alive[4] = {0};
        for (int i = 0; i != 4; ++i) {
            if (!if_tank_dead[i]) {
                Color c1 = Color(i / 2), c2 = Color(1 - c1);
                int dist = field_info.dist_to_shoot_base(i, field_map);
                single_invade[i] = double(100) / (dist * dist) +
                                   argv[3] * field_info.area_fire(i, field_map);
                single_alive[i] = argv[5];
                for (int j = 0; j != 2; ++j) {
                    int k = (c2 << 1) + j;
                    single_invade[i] += argv[4] * single_threat[i][k];
                    single_alive[i] += single_threat[k][i];
                }
            }

        }
        for (int i = 0; i != 4; ++i) {
            single_invade[i] = pow(single_invade[i], argv[6]);
            if (single_alive[i] != 0) {
                int area_move = field_info.area_move(i, field_map);
                single_alive[i] = ((8 - area_move) * area_move + argv[7]) / single_alive[i];
            }
        }


        // calc tank associated values
        double single_attack[4] = {0}, single_defend[4] = {0};
        for (int i = 0; i != 4; ++i) {
            single_attack[i] = single_invade[i] * single_alive[i];
            for (int j = 0; j != 2; ++j) {
                int k = ((1 - i / 2) << 1) + j;
                single_defend[i] += single_invade[k] * single_counter[i][k];
            }
        }

        double single_tank[4];
        for (int i = 0; i != 4; ++i) {
            single_tank[i] = sqrt(single_attack[i] * single_attack[i] + argv[8] * single_defend[i] * single_defend[i]);
        }


        double blue = single_tank[0] + single_tank[1];
        // double tank buff
        if (!if_tank_dead[0] && !if_tank_dead[1]) {
            blue += argv[9] * sqrt(single_invade[0] * single_invade[1]) /
                    field_info.dist_to_tank(0, 1, field_map);
        }

        double red = single_tank[2] + single_tank[3];
        // double tank buff
        if (!if_tank_dead[2] && !if_tank_dead[3]) {
            red += argv[9] * sqrt(single_invade[2] * single_invade[3]) /
                   field_info.dist_to_tank(2, 3, field_map);
        }

        // 输出各项数值
        if (if_print) {
            std::cout << "single_threat";
            for (int i = 0; i != 4; ++i) {
                for (int j = 0; j != 4; ++j) {
                    std::cout << ' ' << single_threat[i][j];
                }
            }
            std::cout << std::endl;

            std::cout << "single_counter";
            for (int i = 0; i != 4; ++i) {
                for (int j = 0; j != 4; ++j) {
                    std::cout << ' ' << single_counter[i][j];
                }
            }
            std::cout << std::endl;

            std::cout << "single_invade";
            for (int i = 0; i != 4; ++i) {
                std::cout << ' ' << single_invade[i];
            }
            std::cout << std::endl;

            std::cout << "single_alive";
            for (int i = 0; i != 4; ++i) {
                std::cout << ' ' << single_alive[i];
            }
            std::cout << std::endl;

            std::cout << "single_attack";
            for (int i = 0; i != 4; ++i) {
                std::cout << ' ' << single_attack[i];
            }
            std::cout << std::endl;

            std::cout << "single_defend";
            for (int i = 0; i != 4; ++i) {
                std::cout << ' ' << single_defend[i];
            }
            std::cout << std::endl;

            std::cout << "single_tank";
            for (int i = 0; i != 4; ++i) {
                std::cout << ' ' << single_tank[i];
            }
            std::cout << std::endl;

            std::cout << blue << ' ' << red << std::endl;
            std::cout << blue / (blue + red) << std::endl;
        }


        return blue / (blue + red);
    }

    bool is_avail(int tank, Move m) const {
        return field_map.is_avail(tank, m);
    }

    bool is_tank_alive(Color color, int i) const {
        return field_map.get_tank((color << 1) + i) != Null_pos;
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
