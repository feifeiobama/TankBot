cd ./data/
rm -rf ./*
cmake -DCMAKE_BUILD_TYPE=Release ..
make clean;
make Tank_mcts_lib;
