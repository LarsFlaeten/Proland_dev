set(LIBNAME "proland-edit")

# Sources
message(STATUS "****    Proland edit    ****")
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
set(PROLAND_EDIT_SOURCES "${PROJECT_SOURCE_DIR}/edit/sources" CACHE INTERNAL "")
message(STATUS "PROLAND_EDIT_SOURCES: ${PROLAND_EDIT_SOURCES}")

include_directories(${PROLAND_CORE_SOURCES} ${PROJECT_SOURCE_DIR}/libraries ${PROLAND_TERRAIN_SOURCES} ${PROLAND_GRAPH_SOURCES} ${PROLAND_RIVER_SOURCES} ${PROLAND_EDIT_SOURCES})

file(GLOB SOURCE_FILES *.cpp */*.cpp ui/twbar/*.cpp)

# Libraries
set(LIBS "")
if(UNIX)
	set(LIBS ${LIBS} rt)
endif(UNIX)

# Static or shared?
set(LIBTYPE STATIC)
if(BUILD_SHARED)
	set(LIBTYPE SHARED)
endif(BUILD_SHARED)

# Create a static library
add_library(${LIBNAME} ${LIBTYPE} ${SOURCE_FILES})
target_link_libraries(${LIBNAME} ${LIBS})

# Adds SO Version and subversion. To be added to ensure ABI/API compatibility.
#SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES SOVERSION ${ORK_VERSION_MAJOR} VERSION ${ORK_VERSION})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message(STATUS "Output path for " ${LIBNAME} " is " ${LIBRARY_OUTPUT_PATH})
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Install headers
FOREACH(subdir edit ui/twbar)
  FILE(GLOB include-files ${subdir}/*.h)
  INSTALL(FILES ${include-files} DESTINATION include/proland/${subdir})
ENDFOREACH(subdir)

# Install library
INSTALL(TARGETS ${LIBNAME} LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

#SET(ORK_CFLAGS "-DORK_API= -DTIXML_USE_STL")
#SET(PROLAND_CFLAGS "-DPROLAND_API=" "-DUSE_SHARED_PTR")

CONFIGURE_FILE(proland-edit.pc.in ${PROJECT_BINARY_DIR}/proland-edit.pc @ONLY)
INSTALL(FILES ${PROJECT_BINARY_DIR}/proland-edit.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
