//
// Created by szc on 2018/10/16.
//

#include "IO_handler.hpp"
#include "Minimax_players.hpp"
#include "Game.hpp"
#include "MCTS_player.hpp"
#include <iostream>

extern "C" {
void game_start() {
    std::ios::sync_with_stdio(false);

    Field_map field_map = Field_map(false);
    Color currentColor = recover_from_input(field_map);
    Node node = Node(field_map);

    MCTS_player player = MCTS_player(currentColor);
    Action action = player.make_decision(node);
    encode_output(action);
}
}