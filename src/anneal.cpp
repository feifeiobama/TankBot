//
// Created by szc on 2018/10/15.
//

#include "Field.hpp"
#include "Minimax_players.hpp"
#include <iostream>
#include <random>
#include <iomanip>

double mutation_range = 0.5;
double decay_rate = 0.995;
int max_step = 40;
int play_round = 10;

class Play_ground {
    double players[3][Argc];
    double mutate_player[Argc];
public:
    void read_player() {
        for (int player = 0; player != 3; ++player) {
            for (int i = 0; i != Argc; ++i) {
                std::cin >> players[player][i];
            }
        }

        std::cout << "There are " << Argc << " args" << std::endl;
        for (int player = 0; player != 3; ++player) {
            print_player(player);
        }
    }

    void print_player(int player) {
        std::cout << "Player " << player << ": ";
        for (int i = 0; i != Argc; ++i) {
            std::cout << std::fixed << std::setprecision(10) << std::right << std::setw(14) << players[player][i]
                      << ' ';
        }
        std::cout << std::endl;
    }

    void anneal() {


        std::cout << "anneal finished" << std::endl;
    }

    int multi_simulate(int player1, int player2) {
        time_t start = clock();

        int score[2] = {0};
        Minimax_players greedy_player1 = Minimax_players(players[player1]),
                greedy_player2 = Minimax_players(players[player2]);

        for (int i = 0; i != play_round; ++i) {
            int result = single_simulate(player1, player2, greedy_player1, greedy_player2);
            if (result == -1) {
                score[0] += 1;
            } else if (result == 0) {
                score[1] += 1;
            }
        }
        std::cout << "SUMMARY: " << score[0] << ":" << score[1] << ", ";
        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
        std::cout << std::fixed << std::setprecision(4) << sec << " s." << std::endl;
        return score[0] - score[1];
    }

    int single_simulate(int player1, int player2, Minimax_players &greedy_player1, Minimax_players &greedy_player2) {
        time_t start = clock();

        Field field = Field();
        int result = 2;
        while (result == 2) {
            Action action1 = greedy_player1.make_decision(BLUE, field);
            Action action2 = greedy_player2.make_decision(RED, field);
            result = field.update(action1, action2);
        }

        if (result == 1) {
            std::cout << "Draw, ";
        } else {
            int win_player = result == -1 ? player1 : player2;
            std::cout << "Player " << win_player << " won, ";
        }

        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
        std::cout << field.get_round() << " rounds, ";
        std::cout << std::fixed << std::setprecision(4) << sec << " s." << std::endl;

        return result;
    }
};

int main() {
    Play_ground play_ground = Play_ground();

    std::ios::sync_with_stdio(false);
    play_ground.read_player();
    play_ground.anneal();
}
