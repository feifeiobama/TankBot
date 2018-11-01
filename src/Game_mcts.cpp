//
// Created by szc on 2018/10/16.
//

#include "IO_handler.hpp"
#include "Minimax_players.hpp"
#include "Game.hpp"
#include "MCTS_player.hpp"
#include <iostream>

using namespace std;

extern "C" {
void game_start() {
    ios::sync_with_stdio(false);

    vector<pair<Field_map, Action> > history[2];
    Field_map field_map = Field(false);
    Color currentColor = recover_from_input(field_map, history);
    Node node = Node(field_map);

    MCTS_player player = MCTS_player(currentColor);
    Action action = player.make_decision(node);
    encode_output(action);
}
}