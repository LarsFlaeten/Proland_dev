cmake_minimum_required(VERSION 2.6)

set(EXENAME core-helloworld)

# Sources
include_directories("${PROLAND_CORE_SOURCES}")
file(GLOB SOURCE_FILES *.cpp)

add_definitions("-DORK_API=")

# For these examples, we make the exeuctable in the directory itself
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Setting example output dir: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(${EXENAME} ${SOURCE_FILES})
target_link_libraries(${EXENAME} -Wl,--whole-archive proland-core ork -Wl,--no-whole-archive pthread GL GLU GLEW glut glfw3 rt dl Xrandr Xinerama Xxf86vm Xext Xcursor Xrender Xfixes X11 stb_image tinyxml)

