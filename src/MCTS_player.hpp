//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_NODE_HPP
#define PROJECT_NODE_HPP

#include "def.hpp"
#include "Field.hpp"
#include <map>
#include <string>
#include <sstream>

using namespace std;

const double MAX_TIME = 4.5;
const int SEARCH_UPPER_BOUND = 10000000;

class Val_table {
    double win_rate = 0;
    int calcd = 0;
public:
    Val_table() {}

    double precedence(int father, double rate = 1.0) {
        if (calcd == 0) {
            return drand48() + rate * 10;
        } else {
            return win_rate + rate * sqrt(2 * log(father) / double(calcd));
        }
    }

    double get_win_rate() { return win_rate; }

    int get_calcd() { return calcd; }

    // 0 draw 1 win -1 lose
    void update(double is_win) {
        calcd += 1;
        win_rate = (win_rate * (calcd - 1) + is_win) / calcd;
    }
};

class Node {
    int offset; // 0 or 2
    Field field;

    Val_table *my_val_table[2][9]; // 记录己方胜率
    Val_table *en_val_table[9][9][2][9]; // 记录敌方胜率
    bool has_cache = false;

    map<int, Node *> children; // key 也是己方：敌方
    Val_table val = Val_table(); // 记录相对胜率
    int to_select[4] = {-1, -1, -1, -1}; // 己方敌方 [-1, 7]
    Node *father = NULL;
public:
    Node(const Field_map &field_map, Color color_) : field(field_map) {
        offset = color_ == BLUE ? 0 : 2;
        val.update(evaluate());
    }

    Node(Node *father_) : field(father_->field), father(father_), offset(father_->offset) {};

    void calc_avail_move() {
        if (has_cache) {
            return;
        }
        for (int i = 0; i != 2; ++i) {
            for (int j = 0; j != 9; ++j) {
                if (field.is_avail(i + offset, j - 1)) {
                    my_val_table[i][j] = new Val_table();
                } else {
                    my_val_table[i][j] = NULL;
                }
            }
        }
        bool en_avail[2][9] = {0};
        for (int i = 0; i != 2; ++i) {
            for (int j = 0; j != 9; ++j) {
                en_avail[i][j] = field.is_avail((i + offset + 2) % 4, j - 1);
            }
        }
        for (int i = 0; i != 9; ++i) {
            for (int j = 0; j != 9; ++j) {
                if (my_val_table[0][i] == NULL || my_val_table[1][j] == NULL) {
                    continue;
                }
                for (int t = 0; t != 2; ++t) {
                    for (int k = 0; k != 9; ++k) {
                        if (en_avail[t][k]) {
                            en_val_table[i][j][t][k] = new Val_table();
                        } else {
                            en_val_table[i][j][t][k] = NULL;
                        }
                    }
                }
            }
        }
        has_cache = true;
    }

    void update(int m[4]) {
        field.update(Action{m[offset], m[offset + 1]}, Action{m[(offset + 2) % 4], m[(offset + 3) % 4]});
//        field.print();
    }

    Node *select() {
        calc_avail_move();
        if (field.judge() != 2) {
            return this;
        } else {
            for (int i = 0; i != 2; ++i) {
                double max_precedence = 0.0;
                for (int j = 0; j != 9; ++j) {
                    if (my_val_table[i][j] == NULL) {
                        continue;
                    }
                    double precedence = my_val_table[i][j]->precedence(val.get_calcd());
                    if (precedence > max_precedence) {
                        max_precedence = precedence;
                        to_select[i] = j - 1;
                    }
                }
            }
            int sum = 0;
            for (int i = 0; i != 2; ++i) {
                double max_precedence = 0.0;
                for (int j = 0; j != 9; ++j) {
                    if (en_val_table[to_select[0] + 1][to_select[1] + 1][i][j] == NULL) {
                        continue;
                    }
                    sum += en_val_table[to_select[0] + 1][to_select[1] + 1][i][j]->get_calcd();
                }
            }
            for (int i = 0; i != 2; ++i) {
                double max_precedence = 0.0;
                for (int j = 0; j != 9; ++j) {
                    if (en_val_table[to_select[0] + 1][to_select[1] + 1][i][j] == NULL) {
                        continue;
                    }
                    double precedence = en_val_table[to_select[0] + 1][to_select[1] + 1][i][j]->precedence(
                           sum + 1);
                    if (precedence > max_precedence) {
                        max_precedence = precedence;
                        to_select[i + 2] = j - 1;
                    }
                }
            }

            int s = encode(to_select[0], to_select[1], to_select[2], to_select[3]);
            auto p = children.find(s);
            if (p != children.end()) {
                return p->second->select();
            } else {
                return this;
            }
        }
    }

    Node *expand() {
        if (field.judge() != 2) {
            return this;
        } else {
            int s = encode(to_select[0], to_select[1], to_select[2], to_select[3]);
            Node *ptr = new Node(this);
            ptr->update(to_select);
            children.insert(make_pair(s, ptr));
            return ptr;
        }
    }

    void back_propagate(double ans) {
        val.update(ans);
        if (field.judge() == 2 && val.get_calcd() != 1) {
            for (int i = 0; i != 2; ++i) {
                double a = offset == 0 ? ans : 1 - ans;
                my_val_table[i][to_select[i] + 1]->update(a);
                en_val_table[to_select[0] + 1][to_select[1] + 1][i][to_select[i + 2] + 1]->update(1 - a);
            }
        }
        if (father != NULL) {
            father->back_propagate(ans);
        }
    }

    double evaluate() { return field.evaluate(); }

    Action get_ans() {
        Move m[2] = {-1, -1};

        for (int i = 0; i != 2; ++i) {
            double max_win_rate = 0.0;
            for (int j = 0; j != 9; ++j) {
                if (my_val_table[i][j] == NULL) {
                    continue;
                }
#ifndef _BOTZONE_ONLINE
                cout << i << " " << j << " " << my_val_table[i][j]->get_win_rate() << " " << my_val_table[i][j]->get_calcd() << endl;
#endif
                if (my_val_table[i][j]->get_win_rate() > max_win_rate) {
                    max_win_rate = my_val_table[i][j]->get_win_rate();
                    m[i] = j - 1;
                }
            }
        }

        return Action{m[0], m[1]};
    }

    void remove() {
        for (auto p: children) {
            p.second->remove();
            delete p.second;
        }
        for (int i = 0; i != 2; ++i) {
            for (int j = 0; j != 9; ++j) {
                if (my_val_table[i][j] == NULL) {
                    continue;
                }
                delete my_val_table[i][j];
            }
        }
        for (int i = 0; i != 9; ++i) {
            for (int j = 0; j != 9; ++j) {
                for (int t = 0; t != 2; ++t) {
                    for (int k = 0; k != 9; ++k) {
                        if (en_val_table[i][j][t][k] == NULL) {
                            continue;
                        }
                        delete en_val_table[i][j][t][k];
                    }
                }
            }
        }
    }

    int get_calcd() { return val.get_calcd(); }

    double get_win_rate() { return val.get_win_rate(); }
};

class MCTS_player {
    Color color;
    Node *root;
public:
    MCTS_player(Color color_) : color(color_) {}

    ~MCTS_player() {
#ifndef _BOTZONE_ONLINE
        root->remove();
        delete root;
#endif
    }

    Action make_decision(Field_map &field_map) {
        clock_t clk = clock() + clock_t(CLOCKS_PER_SEC * MAX_TIME);
        root = new Node(field_map, color);
        int search_cnt = 0;
        while (search_cnt < SEARCH_UPPER_BOUND && clock() < clk) {
            Node *existing_node = root->select();
            Node *new_node = existing_node->expand();
            new_node->back_propagate(new_node->evaluate());
        }
        return root->get_ans();
    }

    string encode_debug() {
        string s = "";
        ostringstream ss;
        ss << root->get_calcd() << " " << root->get_win_rate();
        s = ss.str();
        return s;
    }
};

#endif //PROJECT_NODE_HPP
