cmake -DCMAKE_BUILD_TYPE=Release .
make clean;
make Tank_minimax_lib;
mv libTank_minimax_lib.so ./data/libTank_minimax_lib.so;
