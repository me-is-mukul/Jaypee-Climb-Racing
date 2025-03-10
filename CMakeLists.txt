cmake_minimum_required(VERSION 3.10)
project(Jaypee_Climb_Racing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(raylib REQUIRED)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(Jaypee_Climb_Racing ${SOURCES})
target_link_libraries(Jaypee_Climb_Racing raylib)

