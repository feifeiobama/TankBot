import ctypes

lib = ctypes.cdll.LoadLibrary("./data/Tank_greedy_library.so")
lib.game_start()
