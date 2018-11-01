cmake -DCMAKE_BUILD_TYPE=Release .
make clean;
make Tank_minimax_library;
mv Tank_minimax_library.so ./data/Tank_minimax_library.so;
