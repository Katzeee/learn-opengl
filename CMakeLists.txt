cmake_minimum_required(VERSION 3.22)


project(learn-opengl CXX)

IF (WIN32)
# add_definitions(-DWIN32)
	MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
ENDIF ()

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif()

set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)

add_subdirectory(create-window)
add_subdirectory(simple-triangle)



#################################################

# cmake_minimum_required(VERSION 3.5)
# project(myproject)

# set(CMAKE_CXX_STANDARD 11)

# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})
# link_libraries(${OPENGL_LIBRARIES})

# find_package(GLEW REQUIRED)
# include_directories(${GLEW_INCLUDE_DIRS})
# link_libraries(${GLEW_LIBRARIES})

# set(SOURCE_FILES main.cpp)
# add_executable(myproject ${SOURCE_FILES})

# target_link_libraries(myproject glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)