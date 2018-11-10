//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_FIELD_HPP
#define PROJECT_FIELD_HPP


#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include "def.hpp"
#include "Field_map.h"
#include "Field_info.h"

constexpr int Argc = 8;
constexpr double Argv[Argc] = {3.8210216365, 0.1034557189, 0.0532674934, 0.0027350287, 0.0142413549, 0.0057076207,
                               10.7704850556, 0.2051516580};
class Field {
    Field_map field_map;
    Field_info field_info;
    int ending = -2;
public:
    Field(bool random_initialize = true) : field_map(random_initialize), field_info(field_map) {}

    Field(const Field_map &field_map1) : field_map(field_map1), field_info(field_map1) {
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
            if (ending == 2) {
                ending = field_info.judge();
            }
            return ending;
        }
    }

    // 返回蓝方胜率 (0,1) 禁止已经结束的局进入
    double evaluate(double const argv[Argc] = Argv, bool if_print = false) {
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
        field_info.mask_tank(if_tank_dead);

        // prepare values

        unsigned dist_to_shoot_base[4];
        pair<int, unsigned> dist_to_shoot_avoid[4][4];
        pair<int, unsigned> dist_to_shoot_avoid_both[4];
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
                dist_to_shoot_avoid_both[i] = field_info.dist_to_shoot_avoid_both(i, field_map);
            }
        }

        // calc basic values
        double score[2] = {0.0, 0.0};

        for (int c = 0; c != 2; ++c) {
            for (int k = 0; k != 2; ++k) {
                int i = (c << 1) + k;
                if (!if_tank_dead[i]) {
                    score[c] += argv[0];
                    score[c] -= argv[1] * int(dist_to_shoot_base[i]);

                    int min_ahead = INT_MIN;
                    if (has_both[1 - c]) {
                        min_ahead = dist_to_shoot_avoid_both[i].first;
                    } else {
                        FOR_ENEMY(i, j, {
                            if (!if_tank_dead[j]) {
                                if (min_ahead < dist_to_shoot_avoid[i][j].first) {
                                    min_ahead = dist_to_shoot_avoid[i][j].first;
                                }
                                break;
                            }
                        })
                    }
                    score[c] += argv[2] * min_ahead;
                    score[c] += argv[3] * area_fire[i];
                    score[c] += argv[4] * area_move[i];
                }
            }
            if (has_both[c]) {
                int i = (c << 1);

                unsigned min_threat = unsigned(-1);
                FOR_ENEMY(i, j, {
                    if (!if_tank_dead[j]) {
                        if (min_threat > dist_to_shoot_after[j].second) {
                            min_threat = dist_to_shoot_after[j].second;
                        }
                    }
                })
                score[c] -= argv[5] * min_threat;
            }
        }

        // add kill judge

        unsigned min_step = unsigned(-1);
        for (int i = 0; i != 6; ++i) {
            if (i >= 4 && (!has_both[0] || if_tank_dead[i - 2])) {
                continue;
            }

            unsigned max_step = 0;
            for (int j = 0; j != 6; ++j) {
                if (j >= 4 && (!has_both[1] || if_tank_dead[j - 4])) {
                    continue;
                }

                if (i >= 4 || j >= 4) {
                    unsigned dist1 = unsigned(-1), dist2 = unsigned(-1);
                    if (i >= 4) {
                        int tank_id = i - 2;
                        int another_tank_id = (tank_id / 2) * 2 + (1 - tank_id % 2);
                        dist1 = dist_to_shoot_after[tank_id].first;
                        if (!if_tank_dead[another_tank_id]) {
                            dist2 = dist_to_shoot_base[another_tank_id];
                        }
                        if (dist_to_shoot_after[tank_id].second > 1 &&
                            dist2 > dist_to_shoot_avoid_both[tank_id].second) {
                            dist2 = dist_to_shoot_avoid_both[tank_id].second;
                        }
                    }
                    if (j >= 4) {
                        int tank_id = j - 4;
                        int another_tank_id = (tank_id / 2) * 2 + (1 - tank_id % 2);
                        if (dist2 > dist_to_shoot_after[tank_id].first) {
                            dist2 = dist_to_shoot_after[tank_id].first;
                        }
                        if (!if_tank_dead[another_tank_id] && dist1 > dist_to_shoot_base[another_tank_id]) {
                            dist1 = dist_to_shoot_base[another_tank_id];
                        }
                        if (dist_to_shoot_after[tank_id].second > 1 &&
                            dist1 > dist_to_shoot_avoid_both[tank_id].second) {
                            dist1 = dist_to_shoot_avoid_both[tank_id].second;
                        }
                    }
                    if (dist1 >= dist2) {
                        goto skip_first_loop;
                    } else {
                        if (dist1 > max_step) {
                            max_step = dist1;
                        }
                    }
                } else {
                    int action[4] = {i % 2, i / 2, j % 2, j / 2};
                    unsigned dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)};
                    for (int t = 0; t != 4; ++t) {
                        if (if_tank_dead[t]) {
                            continue;
                        }
                        if (action[t] != 1) {
                            continue;
                        }
                        dist[t] = dist_to_shoot_base[t];

                        bool both_defend = true;
                        FOR_ENEMY(t, e, {
                            if (if_tank_dead[e]) {
                                both_defend = false;
                                continue;
                            }
                            if (action[e] == 1) {
                                both_defend = false;
                                continue;
                            }
                            if (dist_to_shoot_avoid[t][e].second > dist[t]) {
                                dist[t] = dist_to_shoot_avoid[t][e].second;
                            }
                        })

                        if (both_defend) {
                            dist[t] = dist_to_shoot_avoid_both[t].second;
                        }
                    }
                    unsigned min_dist1 = min(dist[0], dist[1]), min_dist2 = min(dist[2], dist[3]);
                    if (min_dist1 >= min_dist2) {
                        goto skip_first_loop;
                    } else {
                        if (min_dist1 > max_step) {
                            max_step = min_dist1;
                        }
                    }
                }
            }
            if (min_step > max_step) {
                min_step = max_step;
            }
            skip_first_loop:;
        }
        if (if_print) {
            cout << "blue kill " << int(min_step) << endl;
        }
        if (min_step != unsigned(-1) && field_map.get_round() + min_step <= 100) {
            score[0] += (argv[6] - argv[7] * min_step);
            goto skip_red;
        }

        min_step = unsigned(-1);
        for (int i = 0; i != 6; ++i) {
            if (i >= 4 && (!has_both[1] || if_tank_dead[i - 4])) {
                continue;
            }

            unsigned max_step = 0;
            for (int j = 0; j != 6; ++j) {
                if (j >= 4 && (!has_both[0] || if_tank_dead[j - 2])) {
                    continue;
                }

                if (i >= 4 && j >= 4) {
                    unsigned dist1 = dist_to_shoot_after[i - 4].second,
                            dist2 = dist_to_shoot_after[j - 2].second;
                    if (dist1 >= dist2) {
                        goto skip_second_loop;
                    } else {
                        unsigned step = dist_to_shoot_after[i - 4].first;
                        if (step > max_step) {
                            max_step = step;
                        }
                    }
                } else if (i >= 4 || j >= 4) {
                    unsigned dist1 = unsigned(-1), dist2 = unsigned(-1);
                    if (i >= 4) {
                        int tank_id = i - 4;
                        int another_tank_id = (tank_id / 2) * 2 + (1 - tank_id % 2);
                        dist1 = dist_to_shoot_after[tank_id].first;
                        if (!if_tank_dead[another_tank_id]) {
                            dist2 = dist_to_shoot_base[another_tank_id];
                        }
                        if (dist2 > dist_to_shoot_avoid_both[tank_id].second) {
                            dist2 = dist_to_shoot_avoid_both[tank_id].second;
                        }
                    } else {
                        int tank_id = j - 2;
                        int another_tank_id = (tank_id / 2) * 2 + (1 - tank_id % 2);
                        dist2 = dist_to_shoot_after[tank_id].first;
                        if (!if_tank_dead[another_tank_id]) {
                            dist1 = dist_to_shoot_base[another_tank_id];
                        }
                        if (dist1 > dist_to_shoot_avoid_both[tank_id].second) {
                            dist1 = dist_to_shoot_avoid_both[tank_id].second;
                        }
                    }

                    if (dist1 >= dist2) {
                        goto skip_second_loop;
                    } else {
                        if (dist1 > max_step) {
                            max_step = dist1;
                        }
                    }
                } else {
                    int action[4] = {j % 2, j / 2, i % 2, i / 2};
                    unsigned dist[4] = {unsigned(-1), unsigned(-1), unsigned(-1), unsigned(-1)};
                    for (int t = 0; t != 4; ++t) {
                        if (if_tank_dead[t]) {
                            continue;
                        }
                        if (action[t] != 1) {
                            continue;
                        }
                        dist[t] = dist_to_shoot_base[t];

                        bool both_defend = true;
                        FOR_ENEMY(t, e, {
                            if (if_tank_dead[e]) {
                                both_defend = false;
                                continue;
                            }
                            if (action[e] == 1) {
                                both_defend = false;
                                continue;
                            }
                            if (dist_to_shoot_avoid[t][e].second > dist[t]) {
                                dist[t] = dist_to_shoot_avoid[t][e].second;
                            }
                        })

                        if (both_defend) {
                            dist[t] = dist_to_shoot_avoid_both[t].second;
                        }
                    }


                    unsigned min_dist1 = min(dist[0], dist[1]), min_dist2 = min(dist[2], dist[3]);
                    if (min_dist1 <= min_dist2) {
                        goto skip_second_loop;
                    } else {
                        if (min_dist2 > max_step) {
                            max_step = min_dist2;
                        }
                    }
                }
            }
            if (min_step > max_step) {
                min_step = max_step;
            }
            skip_second_loop:;
        }
        if (if_print) {
            cout << "red kill " << int(min_step) << endl;
        }
        if (min_step != unsigned(-1) && field_map.get_round() + min_step <= 100) {
            score[1] += (argv[6] - argv[7] * min_step);
        }

        skip_red:;

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
