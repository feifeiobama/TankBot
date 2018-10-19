//
// Created by szc on 2018/10/15.
//

#ifndef PROJECT_NODE_HPP
#define PROJECT_NODE_HPP

#include "def.hpp"
#include "Field.hpp"

class Node {
    Field field;

public:
    Node() {}

    Node(const Field_map &field_map1): field(field_map1) {}

    Node(const Node &node) : field(node.field) {}

    int update(Action action1, Action action2) {
        return field.update(action1, action2);
    }

    double value(int argc, double argv[]) {
        return 0;
    }
};

class MCTS_player {
public:
    MCTS_player(Color color) {}

    Action make_decision(Node &node) {return Action{0, 0};}
};

#endif //PROJECT_NODE_HPP
