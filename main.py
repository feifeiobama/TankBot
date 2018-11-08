import ctypes

lib = ctypes.cdll.LoadLibrary("./data/libTank_mix_lib.so")
lib.game_start()
