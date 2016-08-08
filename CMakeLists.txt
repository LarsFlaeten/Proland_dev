project(Proland CXX C)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)


SET(PROLAND_VERSION_MAJOR 4)
SET(PROLAND_VERSION_MINOR 0)
SET(PROLAND_VERSION "${PROLAND_VERSION_MAJOR}.${PROLAND_VERSION_MINOR}")

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\nmkdir build; cd build; cmake ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)

# Add a sensible build type default and warning because empty means no optimization and no debug info.
if(NOT CMAKE_BUILD_TYPE)
	message("WARNING: CMAKE_BUILD_TYPE is not defined!\n         Defaulting to CMAKE_BUILD_TYPE=RelWithDebInfo. Use ccmake to set a proper value.")
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Common deps
find_package(PkgConfig)
#find_package(ork REQUIRED)

PKG_CHECK_MODULES(ORK REQUIRED ork)
include_directories(${ORK_INCLUDE_DIRS})
link_directories(${ORK_LIBRARY_DIRS})
add_definitions(${ORK_CFLAGS})
message( STATUS "Found ork? " ${ORK_FOUND})
message(STATUS "ork include_directories: " ${ORK_INCLUDE_DIRS})
message(STATUS "ork link_directories:    " ${ORK_LIBRARY_DIRS})
message(STATUS "ork add_definitions:     " ${ORK_CFLAGS})

#find_package(GLUT)

set(LIB_INSTALL_DIR "lib" CACHE STRING "Library directory name (lib/lib32/lib64)" )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_definitions("-DPROLAND_API=" "-fPIC")

option(BUILD_SHARED      "Build shared library instead of static"   OFF)
option(BUILD_EXAMPLES    "Build examples"                           ON )
#option(BUILD_TESTS       "Build tests"                              ON )

#General compiler options:
message(STATUS "CXX FLAGS: " ${CMAKE_CXX_FLAGS})

# Sub dirs
#add_subdirectory(libraries)
add_subdirectory(core)
add_subdirectory(terrain)
add_subdirectory(atmo)
#add_subdirectory(ocean)
#add_subdirectory(forest)
add_subdirectory(river)
add_subdirectory(graph)
add_subdirectory(edit)

if(BUILD_EXAMPLES)
	add_subdirectory(core/examples)
    add_subdirectory(terrain/examples)
    add_subdirectory(atmo/examples)
    add_subdirectory(graph/examples)
    add_subdirectory(river/examples)
    add_subdirectory(edit/examples)
endif(BUILD_EXAMPLES)

#if(BUILD_TESTS)
#	add_subdirectory(test)
#endif(BUILD_TESTS)
