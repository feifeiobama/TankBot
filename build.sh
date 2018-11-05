rm -rf data/
mkdir data
cd data/
cmake -DCMAKE_BUILD_TYPE=Release ..
make clean;
make Tank_mcts_lib;
