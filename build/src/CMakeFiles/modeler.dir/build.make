# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/modeler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/modeler/build

# Include any dependencies generated for this target.
include src/CMakeFiles/modeler.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/modeler.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/modeler.dir/flags.make

src/CMakeFiles/modeler.dir/main.cpp.o: src/CMakeFiles/modeler.dir/flags.make
src/CMakeFiles/modeler.dir/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/modeler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/modeler.dir/main.cpp.o"
	cd /root/modeler/build/src && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/modeler.dir/main.cpp.o -c /root/modeler/src/main.cpp

src/CMakeFiles/modeler.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/modeler.dir/main.cpp.i"
	cd /root/modeler/build/src && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/modeler/src/main.cpp > CMakeFiles/modeler.dir/main.cpp.i

src/CMakeFiles/modeler.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/modeler.dir/main.cpp.s"
	cd /root/modeler/build/src && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/modeler/src/main.cpp -o CMakeFiles/modeler.dir/main.cpp.s

src/CMakeFiles/modeler.dir/test.cpp.o: src/CMakeFiles/modeler.dir/flags.make
src/CMakeFiles/modeler.dir/test.cpp.o: ../src/test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/modeler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/modeler.dir/test.cpp.o"
	cd /root/modeler/build/src && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/modeler.dir/test.cpp.o -c /root/modeler/src/test.cpp

src/CMakeFiles/modeler.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/modeler.dir/test.cpp.i"
	cd /root/modeler/build/src && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/modeler/src/test.cpp > CMakeFiles/modeler.dir/test.cpp.i

src/CMakeFiles/modeler.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/modeler.dir/test.cpp.s"
	cd /root/modeler/build/src && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/modeler/src/test.cpp -o CMakeFiles/modeler.dir/test.cpp.s

# Object files for target modeler
modeler_OBJECTS = \
"CMakeFiles/modeler.dir/main.cpp.o" \
"CMakeFiles/modeler.dir/test.cpp.o"

# External object files for target modeler
modeler_EXTERNAL_OBJECTS =

../bin/modeler: src/CMakeFiles/modeler.dir/main.cpp.o
../bin/modeler: src/CMakeFiles/modeler.dir/test.cpp.o
../bin/modeler: src/CMakeFiles/modeler.dir/build.make
../bin/modeler: src/CMakeFiles/modeler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/modeler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/modeler"
	cd /root/modeler/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/modeler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/modeler.dir/build: ../bin/modeler

.PHONY : src/CMakeFiles/modeler.dir/build

src/CMakeFiles/modeler.dir/clean:
	cd /root/modeler/build/src && $(CMAKE_COMMAND) -P CMakeFiles/modeler.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/modeler.dir/clean

src/CMakeFiles/modeler.dir/depend:
	cd /root/modeler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/modeler /root/modeler/src /root/modeler/build /root/modeler/build/src /root/modeler/build/src/CMakeFiles/modeler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/modeler.dir/depend
