# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/lanceb247/progs/hand_annotate

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lanceb247/progs/hand_annotate/@build

# Include any dependencies generated for this target.
include CMakeFiles/swas.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/swas.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/swas.dir/flags.make

CMakeFiles/swas.dir/apps/swas/swas.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/swas/swas.cpp.o: ../apps/swas/swas.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/swas.dir/apps/swas/swas.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/swas/swas.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/swas/swas.cpp

CMakeFiles/swas.dir/apps/swas/swas.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/swas/swas.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/swas/swas.cpp > CMakeFiles/swas.dir/apps/swas/swas.cpp.i

CMakeFiles/swas.dir/apps/swas/swas.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/swas/swas.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/swas/swas.cpp -o CMakeFiles/swas.dir/apps/swas/swas.cpp.s

CMakeFiles/swas.dir/apps/swas/swas.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/swas/swas.cpp.o.requires

CMakeFiles/swas.dir/apps/swas/swas.cpp.o.provides: CMakeFiles/swas.dir/apps/swas/swas.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/swas/swas.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/swas/swas.cpp.o.provides

CMakeFiles/swas.dir/apps/swas/swas.cpp.o.provides.build: CMakeFiles/swas.dir/apps/swas/swas.cpp.o


CMakeFiles/swas.dir/apps/appmisc.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/appmisc.cpp.o: ../apps/appmisc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/swas.dir/apps/appmisc.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/appmisc.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/appmisc.cpp

CMakeFiles/swas.dir/apps/appmisc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/appmisc.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/appmisc.cpp > CMakeFiles/swas.dir/apps/appmisc.cpp.i

CMakeFiles/swas.dir/apps/appmisc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/appmisc.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/appmisc.cpp -o CMakeFiles/swas.dir/apps/appmisc.cpp.s

CMakeFiles/swas.dir/apps/appmisc.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/appmisc.cpp.o.requires

CMakeFiles/swas.dir/apps/appmisc.cpp.o.provides: CMakeFiles/swas.dir/apps/appmisc.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/appmisc.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/appmisc.cpp.o.provides

CMakeFiles/swas.dir/apps/appmisc.cpp.o.provides.build: CMakeFiles/swas.dir/apps/appmisc.cpp.o


CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o: ../apps/shapefile/shapefile.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/shapefile/shapefile.cpp

CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/shapefile/shapefile.cpp > CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.i

CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/shapefile/shapefile.cpp -o CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.s

CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.requires

CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.provides: CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.provides

CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.provides.build: CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o


CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o: ../apps/shapefile/stasm_regex.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/shapefile/stasm_regex.cpp

CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/shapefile/stasm_regex.cpp > CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.i

CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/shapefile/stasm_regex.cpp -o CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.s

CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.requires

CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.provides: CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.provides

CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.provides.build: CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o


CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o: ../apps/swas/fitmeas.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/swas/fitmeas.cpp

CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/swas/fitmeas.cpp > CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.i

CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/swas/fitmeas.cpp -o CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.s

CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.requires

CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.provides: CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.provides

CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.provides.build: CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o


CMakeFiles/swas.dir/apps/swas/fm29.cpp.o: CMakeFiles/swas.dir/flags.make
CMakeFiles/swas.dir/apps/swas/fm29.cpp.o: ../apps/swas/fm29.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/swas.dir/apps/swas/fm29.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/swas.dir/apps/swas/fm29.cpp.o -c /home/lanceb247/progs/hand_annotate/apps/swas/fm29.cpp

CMakeFiles/swas.dir/apps/swas/fm29.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/swas.dir/apps/swas/fm29.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lanceb247/progs/hand_annotate/apps/swas/fm29.cpp > CMakeFiles/swas.dir/apps/swas/fm29.cpp.i

CMakeFiles/swas.dir/apps/swas/fm29.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/swas.dir/apps/swas/fm29.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lanceb247/progs/hand_annotate/apps/swas/fm29.cpp -o CMakeFiles/swas.dir/apps/swas/fm29.cpp.s

CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.requires:

.PHONY : CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.requires

CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.provides: CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.requires
	$(MAKE) -f CMakeFiles/swas.dir/build.make CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.provides.build
.PHONY : CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.provides

CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.provides.build: CMakeFiles/swas.dir/apps/swas/fm29.cpp.o


# Object files for target swas
swas_OBJECTS = \
"CMakeFiles/swas.dir/apps/swas/swas.cpp.o" \
"CMakeFiles/swas.dir/apps/appmisc.cpp.o" \
"CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o" \
"CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o" \
"CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o" \
"CMakeFiles/swas.dir/apps/swas/fm29.cpp.o"

# External object files for target swas
swas_EXTERNAL_OBJECTS =

swas: CMakeFiles/swas.dir/apps/swas/swas.cpp.o
swas: CMakeFiles/swas.dir/apps/appmisc.cpp.o
swas: CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o
swas: CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o
swas: CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o
swas: CMakeFiles/swas.dir/apps/swas/fm29.cpp.o
swas: CMakeFiles/swas.dir/build.make
swas: libstasm.a
swas: /usr/local/lib/libopencv_videostab.so.2.4.13
swas: /usr/local/lib/libopencv_ts.a
swas: /usr/local/lib/libopencv_superres.so.2.4.13
swas: /usr/local/lib/libopencv_stitching.so.2.4.13
swas: /usr/local/lib/libopencv_contrib.so.2.4.13
swas: /usr/local/lib/libopencv_nonfree.so.2.4.13
swas: /usr/local/lib/libopencv_ocl.so.2.4.13
swas: /usr/local/lib/libopencv_gpu.so.2.4.13
swas: /usr/local/lib/libopencv_photo.so.2.4.13
swas: /usr/local/lib/libopencv_objdetect.so.2.4.13
swas: /usr/local/lib/libopencv_legacy.so.2.4.13
swas: /usr/local/lib/libopencv_video.so.2.4.13
swas: /usr/local/lib/libopencv_ml.so.2.4.13
swas: /usr/local/lib/libopencv_calib3d.so.2.4.13
swas: /usr/local/lib/libopencv_features2d.so.2.4.13
swas: /usr/local/lib/libopencv_highgui.so.2.4.13
swas: /usr/local/lib/libopencv_imgproc.so.2.4.13
swas: /usr/local/lib/libopencv_flann.so.2.4.13
swas: /usr/local/lib/libopencv_core.so.2.4.13
swas: CMakeFiles/swas.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lanceb247/progs/hand_annotate/@build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable swas"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/swas.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/swas.dir/build: swas

.PHONY : CMakeFiles/swas.dir/build

CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/swas/swas.cpp.o.requires
CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/appmisc.cpp.o.requires
CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/shapefile/shapefile.cpp.o.requires
CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/shapefile/stasm_regex.cpp.o.requires
CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/swas/fitmeas.cpp.o.requires
CMakeFiles/swas.dir/requires: CMakeFiles/swas.dir/apps/swas/fm29.cpp.o.requires

.PHONY : CMakeFiles/swas.dir/requires

CMakeFiles/swas.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/swas.dir/cmake_clean.cmake
.PHONY : CMakeFiles/swas.dir/clean

CMakeFiles/swas.dir/depend:
	cd /home/lanceb247/progs/hand_annotate/@build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lanceb247/progs/hand_annotate /home/lanceb247/progs/hand_annotate /home/lanceb247/progs/hand_annotate/@build /home/lanceb247/progs/hand_annotate/@build /home/lanceb247/progs/hand_annotate/@build/CMakeFiles/swas.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/swas.dir/depend

