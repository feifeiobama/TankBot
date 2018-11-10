//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_GREEDY_HPP
#define PROJECT_GREEDY_HPP

#include <cstring>
#include "Field.hpp"

class Minimax_players {
    double argv[20];
    double record;
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

    Action make_decision(Color color, Field &field, vector<pair<Field_map, Action> > history[2]) {
        clock_t clk0 = clock();

        // 判定双方可做的动作 (不管怎样 -1 应该都是可以的)
        bool is_avail[4][9];
        for (int i = 0; i != 4; ++i) {
            for (int m = -1; m != 8; ++m) {
                is_avail[i][m + 1] = field.is_avail(i, m);
            }
        }

        Action a = field.find_history(history[1 - color]);
        record = field.evaluate();

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

        clock_t clk1 = clock();
#ifndef _BOTZONE_ONLINE
//        cout << get_val(-1, 6, -1, 4, field, true) << endl;
//        cout << color << endl;
//        printDecision(color, -1, 4, field);
//        printDecision(color, 4, -1, field);
        cout << "time: " << (clk1 - clk0) / double(CLOCKS_PER_SEC) << endl;
#endif
        return ans;
    }

    bool must_end(Color color) {
        cout << record << endl;
        return record >= 0.98 || record <= 0.02;
    }

    void printDecision(Color color, Move m1, Move m2, const Field &field) {
        // 判定双方可做的动作 (不管怎样 -1 应该都是可以的)
        bool is_avail[4][9];
        for (int i = 0; i != 4; ++i) {
            for (int m = -1; m != 8; ++m) {
                is_avail[i][m + 1] = field.is_avail(i, m);
            }
        }

        if (color == BLUE) {
            double min_val = 1.0;
            int m3, m4;
            int best_m3, best_m4;
            for (m3 = -1; m3 != 8; ++m3) {
                if (!is_avail[2][m3 + 1]) {
                    continue;
                }
                for (m4 = -1; m4 != 8; ++m4) {
                    if (!is_avail[3][m4 + 1]) {
                        continue;
                    }
                    double val = get_val(m1, m2, m3, m4, field);
                    if (val < min_val) {
                        min_val = val;
                        best_m3 = m3;
                        best_m4 = m4;
                    }
                }
            }
            get_val(m1, m2, best_m3, best_m4, field, true);
        } else {
            double max_val = 0.0;
            int m3, m4;
            int best_m3, best_m4;
            for (m3 = -1; m3 != 8; ++m3) {
                if (!is_avail[0][m3 + 1]) {
                    continue;
                }
                for (m4 = -1; m4 != 8; ++m4) {
                    if (!is_avail[1][m4 + 1]) {
                        continue;
                    }
                    double val = get_val(m3, m4, m1, m2, field);
                    if (val > max_val) {
                        max_val = val;
                        best_m3 = m3;
                        best_m4 = m4;
                    }
                }
            }
            get_val(best_m3, best_m4, m1, m2, field, true);
        }
    }
};

#endif //PROJECT_GREEDY_HPP
