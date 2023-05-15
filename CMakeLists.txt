cmake_minimum_required(VERSION 3.24)
project(sat_solver)

set(CMAKE_CXX_STANDARD 17)

include_directories(headers)

file(GLOB PROJECT_HEADERS headers/*.h)

file(GLOB PROJECT_SOURCES sources/*.cpp)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})

add_executable(sat_solver main.cpp ${PROJECT_SOURCES} ${PROJECT_HEADERS})
