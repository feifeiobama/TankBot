cmake -DCMAKE_BUILD_TYPE=Release .
make clean;
make Tank_greedy_library;
mv Tank_greedy_library.so ./data/Tank_greedy_library.so;
