import ctypes

lib = ctypes.cdll.LoadLibrary("./data/libTank_mcts_lib.so")
lib.game_start()
