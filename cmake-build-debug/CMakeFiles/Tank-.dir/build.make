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
include CMakeFiles/Tank-.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Tank-.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Tank-.dir/flags.make

CMakeFiles/Tank-.dir/src/main.cpp.o: CMakeFiles/Tank-.dir/flags.make
CMakeFiles/Tank-.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Tank-.dir/src/main.cpp.o"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tank-.dir/src/main.cpp.o -c /Users/szc/Desktop/tank/project/src/main.cpp

CMakeFiles/Tank-.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tank-.dir/src/main.cpp.i"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/szc/Desktop/tank/project/src/main.cpp > CMakeFiles/Tank-.dir/src/main.cpp.i

CMakeFiles/Tank-.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tank-.dir/src/main.cpp.s"
	/Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/szc/Desktop/tank/project/src/main.cpp -o CMakeFiles/Tank-.dir/src/main.cpp.s

CMakeFiles/Tank-.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/Tank-.dir/src/main.cpp.o.requires

CMakeFiles/Tank-.dir/src/main.cpp.o.provides: CMakeFiles/Tank-.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Tank-.dir/build.make CMakeFiles/Tank-.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/Tank-.dir/src/main.cpp.o.provides

CMakeFiles/Tank-.dir/src/main.cpp.o.provides.build: CMakeFiles/Tank-.dir/src/main.cpp.o


# Object files for target Tank-
Tank___OBJECTS = \
"CMakeFiles/Tank-.dir/src/main.cpp.o"

# External object files for target Tank-
Tank___EXTERNAL_OBJECTS =

Tank-: CMakeFiles/Tank-.dir/src/main.cpp.o
Tank-: CMakeFiles/Tank-.dir/build.make
Tank-: CMakeFiles/Tank-.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Tank-"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tank-.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Tank-.dir/build: Tank-

.PHONY : CMakeFiles/Tank-.dir/build

CMakeFiles/Tank-.dir/requires: CMakeFiles/Tank-.dir/src/main.cpp.o.requires

.PHONY : CMakeFiles/Tank-.dir/requires

CMakeFiles/Tank-.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Tank-.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Tank-.dir/clean

CMakeFiles/Tank-.dir/depend:
	cd /Users/szc/Desktop/tank/project/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/szc/Desktop/tank/project /Users/szc/Desktop/tank/project /Users/szc/Desktop/tank/project/cmake-build-debug /Users/szc/Desktop/tank/project/cmake-build-debug /Users/szc/Desktop/tank/project/cmake-build-debug/CMakeFiles/Tank-.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Tank-.dir/depend

