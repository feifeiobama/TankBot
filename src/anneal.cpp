//
// Created by szc on 2018/10/15.
//

#include "Field.hpp"
#include "Minimax_players.hpp"
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <cstdlib>

using namespace std;

double mutation_range = 0.5;
double decay_rate = 0.98;
int max_step = 4;
int play_round = 10;

class Play_ground {
    double players[3][Argc];
    double save_player[Argc];
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
        memcpy(save_player, players[player], Argc * sizeof(double));
        for (int i = 0; i != Argc; ++i) {
            players[player][i] *= pow(2.0, (drand48() - 0.5) * mutation_range);
        }
    }

    void reset(int player) {
        memcpy(players[player], save_player, Argc * sizeof(double));
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
        cout << endl;

        bool tag1, tag2;
        while (true) {
            tag1 = false;
            for (int i = 0; i != max_step; ++i) {
                mutation(0);
                cout << i << ": mutate 0: ";
                int s1 = multi_simulate(0, 1), s2 = multi_simulate(0, 2);
                int sum = s1 + s2 - record[0] - record[1];
                if (drand48() <= double(1) / (1 + exp(-sum))) {
                    mutation_range *= decay_rate;
                    record[0] = s1;
                    record[1] = s2;
                    cout << "accept" << endl;
                    tag1 = true;
                } else {
                    reset(0);
                    cout << "refuse" << endl;
                }
            }
            cout << endl;
            tag2 = false;
            for (int i = 0; i != max_step; ++i) {
                mutation(1);
                cout << i << ": mutate 1: ";
                int s1 = multi_simulate(1, 0), s2 = multi_simulate(1, 2);
                int sum = s1 + s2 + record[0] - record[2];
                if (drand48() <= double(1) / (1 + exp(-sum))) {
                    mutation_range *= decay_rate;
                    record[0] = -s1;
                    record[2] = s2;
                    cout << "accept" << endl;
                    tag2 = true;
                } else {
                    reset(1);
                    cout << "refuse" << endl;
                }
            }
            print_player(0);
            print_player(1);
            cout << record[0] << " " << record[1] << " " << record[2] << endl;
            if (!tag1 && !tag2) {
                break;
            } else {
                tag1 = tag2 = true;
            }
        }
        cout << "anneal finished" << endl;
    }

    int multi_simulate(int player1, int player2) {
        time_t start = clock();

        int score[2] = {0};
        Minimax_players greedy_player1 = Minimax_players(players[player1]),
                greedy_player2 = Minimax_players(players[player2]);

        // cout << "Player " << player1 << " vs Player " << player2;
        for (int i = 0; i != play_round; ++i) {
            int result = single_simulate(player1, player2, greedy_player1, greedy_player2);
            if (result == 0) {
                score[0] += 1;
            } else if (result == 1) {
                score[1] += 1;
            }
        }
        cout << score[0] << ":" << score[1] << " ";
        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
        // cout << fixed << setprecision(4) << sec << " s." << endl;
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

//        if (result == -1) {
//            cout << "Draw, ";
//        } else {
//            int win_player = result == 0 ? player1 : player2;
//            cout << "Player " << win_player << " won, ";
//        }
//
//        double sec = (clock() - start) / double(CLOCKS_PER_SEC);
//        cout << field.get_round() << " rounds, ";
//        cout << fixed << setprecision(4) << sec << " s." << endl;

        return result;
    }
};

int main() {
    ios::sync_with_stdio(false);
    srand(clock());
    srand48(clock());

    Play_ground play_ground = Play_ground();
    play_ground.read_player();
    play_ground.anneal();
}
