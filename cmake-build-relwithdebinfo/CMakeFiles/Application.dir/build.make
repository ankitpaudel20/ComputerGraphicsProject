# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2021.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2021.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\repos\c++\ComputerGraphicsProject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo

# Include any dependencies generated for this target.
include CMakeFiles/Application.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/Application.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Application.dir/flags.make

CMakeFiles/Application.dir/Application/application.cpp.obj: CMakeFiles/Application.dir/flags.make
CMakeFiles/Application.dir/Application/application.cpp.obj: CMakeFiles/Application.dir/includes_CXX.rsp
CMakeFiles/Application.dir/Application/application.cpp.obj: ../Application/application.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Application.dir/Application/application.cpp.obj"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\Application.dir\Application\application.cpp.obj -c D:\repos\c++\ComputerGraphicsProject\Application\application.cpp

CMakeFiles/Application.dir/Application/application.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Application.dir/Application/application.cpp.i"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\repos\c++\ComputerGraphicsProject\Application\application.cpp > CMakeFiles\Application.dir\Application\application.cpp.i

CMakeFiles/Application.dir/Application/application.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Application.dir/Application/application.cpp.s"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\repos\c++\ComputerGraphicsProject\Application\application.cpp -o CMakeFiles\Application.dir\Application\application.cpp.s

CMakeFiles/Application.dir/Application/node.cpp.obj: CMakeFiles/Application.dir/flags.make
CMakeFiles/Application.dir/Application/node.cpp.obj: CMakeFiles/Application.dir/includes_CXX.rsp
CMakeFiles/Application.dir/Application/node.cpp.obj: ../Application/node.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Application.dir/Application/node.cpp.obj"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\Application.dir\Application\node.cpp.obj -c D:\repos\c++\ComputerGraphicsProject\Application\node.cpp

CMakeFiles/Application.dir/Application/node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Application.dir/Application/node.cpp.i"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\repos\c++\ComputerGraphicsProject\Application\node.cpp > CMakeFiles\Application.dir\Application\node.cpp.i

CMakeFiles/Application.dir/Application/node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Application.dir/Application/node.cpp.s"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\repos\c++\ComputerGraphicsProject\Application\node.cpp -o CMakeFiles\Application.dir\Application\node.cpp.s

CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj: CMakeFiles/Application.dir/flags.make
CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj: CMakeFiles/Application.dir/includes_CXX.rsp
CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj: ../Application/vendor/tinyobj/tiny_obj_loader.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\Application.dir\Application\vendor\tinyobj\tiny_obj_loader.cc.obj -c D:\repos\c++\ComputerGraphicsProject\Application\vendor\tinyobj\tiny_obj_loader.cc

CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.i"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\repos\c++\ComputerGraphicsProject\Application\vendor\tinyobj\tiny_obj_loader.cc > CMakeFiles\Application.dir\Application\vendor\tinyobj\tiny_obj_loader.cc.i

CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.s"
	C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\repos\c++\ComputerGraphicsProject\Application\vendor\tinyobj\tiny_obj_loader.cc -o CMakeFiles\Application.dir\Application\vendor\tinyobj\tiny_obj_loader.cc.s

# Object files for target Application
Application_OBJECTS = \
"CMakeFiles/Application.dir/Application/application.cpp.obj" \
"CMakeFiles/Application.dir/Application/node.cpp.obj" \
"CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj"

# External object files for target Application
Application_EXTERNAL_OBJECTS =

Application.exe: CMakeFiles/Application.dir/Application/application.cpp.obj
Application.exe: CMakeFiles/Application.dir/Application/node.cpp.obj
Application.exe: CMakeFiles/Application.dir/Application/vendor/tinyobj/tiny_obj_loader.cc.obj
Application.exe: CMakeFiles/Application.dir/build.make
Application.exe: CMakeFiles/Application.dir/linklibs.rsp
Application.exe: CMakeFiles/Application.dir/objects1.rsp
Application.exe: CMakeFiles/Application.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable Application.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Application.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Application.dir/build: Application.exe
.PHONY : CMakeFiles/Application.dir/build

CMakeFiles/Application.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Application.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Application.dir/clean

CMakeFiles/Application.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\repos\c++\ComputerGraphicsProject D:\repos\c++\ComputerGraphicsProject D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo D:\repos\c++\ComputerGraphicsProject\cmake-build-relwithdebinfo\CMakeFiles\Application.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Application.dir/depend

