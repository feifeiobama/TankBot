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
    Field field = Field(field_map);

    Minimax_players player = Minimax_players();
    Action action = player.make_decision(currentColor, field);
    encode_output(action);
}
}