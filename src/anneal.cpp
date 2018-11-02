//
// Created by szc on 2018/10/15.
//

#include "Field.hpp"
#include "Minimax_players.hpp"
#include <iostream>
#include <random>
#include <iomanip>
#include <cstdlib>

using namespace std;

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
                cin >> players[player][i];
            }
        }

        cout << "There are " << Argc << " args" << endl;
        for (int player = 0; player != 3; ++player) {
            print_player(player);
        }
    }

    void mutation(int player) {
        for (int i = 0; i != Argc; ++i) {
            mutate_player[i] = players[player][i] * pow(2.0, (drand48() - 0.5) * mutation_range);
        }
    }

    void print_player(int player) {
        cout << "Player " << player << ": ";
        for (int i = 0; i != Argc; ++i) {
            cout << fixed << setprecision(10) << right << setw(14) << players[player][i]
                      << ' ';
        }
        cout << endl;
    }

    void anneal() {
        int record[3] = {multi_simulate(0, 1), multi_simulate(0, 2), multi_simulate(1, 2)};

        bool tag1 = true, tag2 = true;
        while(tag1 || tag2) {
            print_player(0);
            print_player(1);
            print_player(2);
        }

        cout << "anneal finished" << endl;
    }

    int multi_simulate(int player1, int player2) {
        time_t start = clock();

        int score[2] = {0};
        Minimax_players greedy_player1 = Minimax_players(players[player1]),
                greedy_player2 = Minimax_players(players[player2]);

        cout << "Player " << player1 << " vs " << player2 << endl;
        for (int i = 0; i != play_round; ++i) {
            int result = single_simulate(player1, player2, greedy_player1, greedy_player2);
            if (result == -1) {
                score[0] += 1;
            } else if (result == 0) {
                score[1] += 1;
            }
        }
        cout << "SUMMARY: " << score[0] << ":" << score[1] << ", ";
        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
        cout << fixed << setprecision(4) << sec << " s." << endl;
        return score[0] - score[1];
    }

    int single_simulate(int player1, int player2, Minimax_players &greedy_player1, Minimax_players &greedy_player2) {
        time_t start = clock();

        vector<pair<Field_map, Action> > history[2];
        Field field = Field();
        int result = 2;
        while (result == 2) {
            Action action1 = greedy_player1.make_decision(BLUE, field, history);
            Action action2 = greedy_player2.make_decision(RED, field, history);
            field.push_history(action1, action2, history);
            result = field.update(action1, action2);
        }

        if (result == 1) {
            cout << "Draw, ";
        } else {
            int win_player = result == -1 ? player1 : player2;
            cout << "Player " << win_player << " won, ";
        }

        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
        cout << field.get_round() << " rounds, ";
        cout << fixed << setprecision(4) << sec << " s." << endl;

        return result;
    }
};

int main() {
    Play_ground play_ground = Play_ground();

    ios::sync_with_stdio(false);
    play_ground.read_player();
    play_ground.anneal();
}
