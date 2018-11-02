import ctypes

lib = ctypes.cdll.LoadLibrary("./data/libTank_minimax_lib.so")
lib.game_start()
