//
// Created by szc on 2018/10/16.
//

#include "IO_handler.hpp"
#include "Minimax_players.hpp"
#include "MCTS_player.hpp"
#include "Game.hpp"
#include <iostream>
#include <random>

using namespace std;

extern "C" {
void game_start() {
    ios::sync_with_stdio(false);
    srand(clock());
    srand48(clock());

    vector<pair<Field_map, Action> > history[2];
    Field_map field_map = Field_map(false);
    Color currentColor = recover_from_input(field_map, history);

    Field field = Field(field_map);
    Minimax_players player1 = Minimax_players();
    Action action1 = player1.make_decision(currentColor, field, history);
    if (player1.must_end(currentColor)) {
        encode_output(action1);
        return;
    }
    MCTS_player player2 = MCTS_player(currentColor);
    Action action2 = player2.make_decision(field_map, history);
    encode_output(action2, player2.encode_debug());
}
}