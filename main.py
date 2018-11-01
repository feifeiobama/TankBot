import ctypes

lib = ctypes.cdll.LoadLibrary("./data/Tank_minimax_library.so")
lib.game_start()
