# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/szc/Desktop/tank/project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/szc/Desktop/tank/project/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Tank_max_min_library.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Tank_max_min_library.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Tank_max_min_library.dir/flags.make

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o: CMakeFiles/Tank_max_min_library.dir/flags.make
CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o: ../src/Game_minimax.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o -c /Users/szc/Desktop/tank/project/src/Game_minimax.cpp

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.i"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/szc/Desktop/tank/project/src/Game_minimax.cpp > CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.i

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.s"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/szc/Desktop/tank/project/src/Game_minimax.cpp -o CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.s

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.requires:

.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.requires

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.provides: CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.requires
	$(MAKE) -f CMakeFiles/Tank_max_min_library.dir/build.make CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.provides.build
.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.provides

CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.provides.build: CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o


CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o: CMakeFiles/Tank_max_min_library.dir/flags.make
CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o: ../src/Field_info.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o -c /Users/szc/Desktop/tank/project/src/Field_info.cpp

CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.i"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/szc/Desktop/tank/project/src/Field_info.cpp > CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.i

CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.s"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/szc/Desktop/tank/project/src/Field_info.cpp -o CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.s

CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.requires:

.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.requires

CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.provides: CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.requires
	$(MAKE) -f CMakeFiles/Tank_max_min_library.dir/build.make CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.provides.build
.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.provides

CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.provides.build: CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o


CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o: CMakeFiles/Tank_max_min_library.dir/flags.make
CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o: ../src/Field_map.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o -c /Users/szc/Desktop/tank/project/src/Field_map.cpp

CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.i"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/szc/Desktop/tank/project/src/Field_map.cpp > CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.i

CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.s"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/szc/Desktop/tank/project/src/Field_map.cpp -o CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.s

CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.requires:

.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.requires

CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.provides: CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.requires
	$(MAKE) -f CMakeFiles/Tank_max_min_library.dir/build.make CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.provides.build
.PHONY : CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.provides

CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.provides.build: CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o


# Object files for target Tank_max_min_library
Tank_max_min_library_OBJECTS = \
"CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o" \
"CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o" \
"CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o"

# External object files for target Tank_max_min_library
Tank_max_min_library_EXTERNAL_OBJECTS =

libTank_max_min_library.dylib: CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o
libTank_max_min_library.dylib: CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o
libTank_max_min_library.dylib: CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o
libTank_max_min_library.dylib: CMakeFiles/Tank_max_min_library.dir/build.make
libTank_max_min_library.dylib: CMakeFiles/Tank_max_min_library.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library libTank_max_min_library.dylib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tank_max_min_library.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Tank_max_min_library.dir/build: libTank_max_min_library.dylib

.PHONY : CMakeFiles/Tank_max_min_library.dir/build

CMakeFiles/Tank_max_min_library.dir/requires: CMakeFiles/Tank_max_min_library.dir/src/Game_minimax.cpp.o.requires
CMakeFiles/Tank_max_min_library.dir/requires: CMakeFiles/Tank_max_min_library.dir/src/Field_info.cpp.o.requires
CMakeFiles/Tank_max_min_library.dir/requires: CMakeFiles/Tank_max_min_library.dir/src/Field_map.cpp.o.requires

.PHONY : CMakeFiles/Tank_max_min_library.dir/requires

CMakeFiles/Tank_max_min_library.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Tank_max_min_library.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Tank_max_min_library.dir/clean

CMakeFiles/Tank_max_min_library.dir/depend:
	cd /Users/szc/Desktop/tank/project/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/szc/Desktop/tank/project /Users/szc/Desktop/tank/project /Users/szc/Desktop/tank/project/cmake-build-debug /Users/szc/Desktop/tank/project/cmake-build-debug /Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles/Tank_max_min_library.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Tank_max_min_library.dir/depend

