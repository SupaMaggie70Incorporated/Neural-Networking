# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking\build"

# Utility rule file for NightlyMemoryCheck.

# Include any custom commands dependencies for this target.
include Othello/CMakeFiles/NightlyMemoryCheck.dir/compiler_depend.make

# Include the progress variables for this target.
include Othello/CMakeFiles/NightlyMemoryCheck.dir/progress.make

Othello/CMakeFiles/NightlyMemoryCheck:
	cd /d C:\Users\SupaM\DOCUME~1\Magnus\CODEWO~1\NEURAL~1\build\Othello && "C:\Program Files\CMake\bin\ctest.exe" -D NightlyMemoryCheck

NightlyMemoryCheck: Othello/CMakeFiles/NightlyMemoryCheck
NightlyMemoryCheck: Othello/CMakeFiles/NightlyMemoryCheck.dir/build.make
.PHONY : NightlyMemoryCheck

# Rule to build all files generated by this target.
Othello/CMakeFiles/NightlyMemoryCheck.dir/build: NightlyMemoryCheck
.PHONY : Othello/CMakeFiles/NightlyMemoryCheck.dir/build

Othello/CMakeFiles/NightlyMemoryCheck.dir/clean:
	cd /d C:\Users\SupaM\DOCUME~1\Magnus\CODEWO~1\NEURAL~1\build\Othello && $(CMAKE_COMMAND) -P CMakeFiles\NightlyMemoryCheck.dir\cmake_clean.cmake
.PHONY : Othello/CMakeFiles/NightlyMemoryCheck.dir/clean

Othello/CMakeFiles/NightlyMemoryCheck.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking" "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking\Othello" "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking\build" "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking\build\Othello" "C:\Users\SupaM\Documents\Magnus\Code Workspaces\Neural Networking\build\Othello\CMakeFiles\NightlyMemoryCheck.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : Othello/CMakeFiles/NightlyMemoryCheck.dir/depend

