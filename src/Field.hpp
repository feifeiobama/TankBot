//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_FIELD_HPP
#define PROJECT_FIELD_HPP

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include "def.hpp"
#include "Field_map.h"
#include "Field_info.h"

constexpr int Argc = 6;
constexpr double Argv[Argc] = {0};

class Field {
    Field_map field_map;
    Field_info field_info;
    int ending = -2;
public:
    Field(bool random_initialize=true) : field_map(random_initialize), field_info(field_map) {}

    Field(const Field_map &field_map1): field_map(field_map1), field_info(field_map1) {
        print();
    }

    Field(const Field &field) : field_map(field.field_map), field_info(field.field_info) {}

    int update(Action action1, Action action2) {
        field_map.update(action1, action2);
        field_info.update(action1, action2, field_map);
        return field_map.judge();
    }

    void push_history(Action &action1, Action &action2, vector<pair<Field_map, Action> > history[2]) const {
        history[0].push_back(make_pair(field_map, action1));
        history[1].push_back(make_pair(field_map, action2));
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
        pair<unsigned, unsigned> dist_to_shoot_after[4];
        unsigned area_fire[4];
        unsigned area_move[4];
        bool has_both[2];

        for (int i = 0; i != 4; ++i) {
            if (if_tank_dead[i]) {
                continue;
            }
            dist_to_shoot_base[i] = field_info.dist_to_shoot_base(i, field_map);
            area_fire[i] = field_info.area_fire(i, field_map);
            area_move[i] = field_info.area_move(i, field_map);

            has_both[1 - (i >> 1)] = true;
            FOR_ENEMY(i, j, {
                if (if_tank_dead[j]) {
                    has_both[1 - (i >> 1)] = false;
                    continue;
                }
                dist_to_shoot_avoid[i][j] = field_info.dist_to_shoot_avoid(i, j, field_map);
            })
            if (has_both[1 - (i >> 1)]) {
                dist_to_shoot_after[i] = field_info.dist_to_shoot_after(i, field_map);
            }
        }

        // calc basic values
        double score[2] = {0};

        for (int c = 0; c != 2; ++c) {
            for (int k = 0; k != 2; ++k) {
                int i = (c << 1) + k;
                if (!if_tank_dead[i]) {
                    score[c] += argv[0];
                    score[c] += argv[1] * dist_to_shoot_base[i];

                    int min_ahead = INT_MIN;
                    FOR_ENEMY(i, j, {
                        if (!if_tank_dead[j]) {
                            if (min_ahead < dist_to_shoot_avoid[i][j].first) {
                                min_ahead = dist_to_shoot_avoid[i][j].first;
                            }
                        }
                    })
                    score[c] -= argv[2] * min_ahead;

                    score[c] += argv[3] * area_fire[i];
                    score[c] += argv[4] * area_move[i];
                }
            }
            if (has_both[c]) {
                int i = (c << 1);

                unsigned min_threat = unsigned(-1);
                FOR_ENEMY(i, j, {
                    if (!if_tank_dead[j]) {
                        if (min_threat < dist_to_shoot_after[j].second) {
                            min_threat = dist_to_shoot_after[j].second;
                        }
                    }
                })
                score[c] -= argv[5] * min_threat;
            }
        }

        // add kill judge


        double eval = double(1) / (1 + exp(score[1] - score[0]));
        if (if_print) {
            cout << score[0] << " " << score[1] << " " << eval << endl;
        }
        return eval;
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
