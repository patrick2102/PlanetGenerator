# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug"

# Include any dependencies generated for this target.
include exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\depend.make

# Include the progress variables for this target.
include exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\progress.make

# Include the compile flags for this target's objects.
include exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\flags.make

exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\main.cpp.obj: exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\flags.make
exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\main.cpp.obj: ..\exercises\exercise_7\exercise_7\main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object exercises/exercise_7/exercise_7/CMakeFiles/exercise_7.dir/main.cpp.obj"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1426~1.288\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\exercise_7.dir\main.cpp.obj /FdCMakeFiles\exercise_7.dir\ /FS -c "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\exercises\exercise_7\exercise_7\main.cpp"
<<
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1

exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise_7.dir/main.cpp.i"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1426~1.288\bin\Hostx86\x86\cl.exe > CMakeFiles\exercise_7.dir\main.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\exercises\exercise_7\exercise_7\main.cpp"
<<
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1

exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise_7.dir/main.cpp.s"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1426~1.288\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\exercise_7.dir\main.cpp.s /c "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\exercises\exercise_7\exercise_7\main.cpp"
<<
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1

# Object files for target exercise_7
exercise_7_OBJECTS = \
"CMakeFiles\exercise_7.dir\main.cpp.obj"

# External object files for target exercise_7
exercise_7_EXTERNAL_OBJECTS =

exercises\exercise_7\exercise_7\exercise_7.exe: exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\main.cpp.obj
exercises\exercise_7\exercise_7\exercise_7.exe: exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\build.make
exercises\exercise_7\exercise_7\exercise_7.exe: common\third-party\glad\glad.lib
exercises\exercise_7\exercise_7\exercise_7.exe: common\third-party\glfw\src\glfw3.lib
exercises\exercise_7\exercise_7\exercise_7.exe: common\third-party\imgui\imgui.lib
exercises\exercise_7\exercise_7\exercise_7.exe: common\third-party\assimp\assimp.lib
exercises\exercise_7\exercise_7\exercise_7.exe: exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable exercise_7.exe"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	"D:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\exercise_7.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\mt.exe --manifests  -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1426~1.288\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\exercise_7.dir\objects1.rsp @<<
 /out:exercise_7.exe /implib:exercise_7.lib /pdb:"C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug\exercises\exercise_7\exercise_7\exercise_7.pdb" /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console  ..\..\..\common\third-party\glad\glad.lib ..\..\..\common\third-party\glfw\src\glfw3.lib ..\..\..\common\third-party\imgui\imgui.lib ..\..\..\common\third-party\assimp\assimp.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Copying shaders"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	echo >nul && "D:\Program Files\JetBrains\CLion 2020.2.3\bin\cmake\win\bin\cmake.exe" -E copy_directory "C:/Uni/Games 2nd Semester/2nd Semester/Graphics Programming/PlanetGenerator/exercises/exercise_7/exercise_7/shaders" "C:/Uni/Games 2nd Semester/2nd Semester/Graphics Programming/PlanetGenerator/cmake-build-debug/exercises/exercise_7/exercise_7/shaders"
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1

# Rule to build all files generated by this target.
exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\build: exercises\exercise_7\exercise_7\exercise_7.exe

.PHONY : exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\build

exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\clean:
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1\EXERCI~1\EXF0D3~1\EXERCI~1
	$(CMAKE_COMMAND) -P CMakeFiles\exercise_7.dir\cmake_clean.cmake
	cd C:\Uni\GAMES2~1\2NDSEM~1\GRAPHI~1\PLANET~1\CMAKE-~1
.PHONY : exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\clean

exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator" "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\exercises\exercise_7\exercise_7" "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug" "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug\exercises\exercise_7\exercise_7" "C:\Uni\Games 2nd Semester\2nd Semester\Graphics Programming\PlanetGenerator\cmake-build-debug\exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : exercises\exercise_7\exercise_7\CMakeFiles\exercise_7.dir\depend

