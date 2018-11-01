cmake_minimum_required(VERSION 3.5)
project(Tank)

set(CMAKE_CXX_FLAGS_RELEASE -O3)
set(CMAKE_CXX_STANDARD 11)

set(base src/def.hpp src/Field_info.cpp src/Field_map.cpp src/Field.hpp)
set(exe_entry src/main.cpp src/Game.hpp)

add_executable(Anneal src/anneal.cpp src/Minimax_players.hpp ${base})
add_executable(Tank_minimax ${exe_entry} src/Game_minimax.cpp src/Minimax_players.hpp src/IO_handler.hpp ${base})
add_library(Tank_minimax_lib SHARED src/Game_minimax.cpp src/Minimax_players.hpp src/IO_handler.hpp ${base})

add_executable(Tank_mcts ${exe_entry} src/Game_mcts.cpp src/MCTS_player.hpp src/IO_handler.hpp ${base})
add_library(Tank_mcts_lib SHARED src/Game_mcts.cpp src/MCTS_player.hpp src/IO_handler.hpp ${base})
