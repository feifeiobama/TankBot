//
// Created by szc on 2018/10/17.
//

#ifndef TANK_FIELD_MAP_H
#define TANK_FIELD_MAP_H

#include "def.hpp"
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
