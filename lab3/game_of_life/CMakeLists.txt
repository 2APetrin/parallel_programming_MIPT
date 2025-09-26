cmake_minimum_required(VERSION 3.9)

project(lab3 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)


add_executable(game_of_life_simulation ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Board.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/src/core/BoardSerializer.cpp)
target_include_directories(game_of_life_simulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
target_link_libraries(game_of_life_simulation PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(game_of_life_simulation PUBLIC Threads::Threads)
target_compile_options(game_of_life_simulation PUBLIC ${OpenMP_CXX_FLAGS})
target_compile_options(game_of_life_simulation PUBLIC -DSIMULATION)


add_executable(game_of_life_parallel ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Board.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/src/core/BoardSerializer.cpp)
target_include_directories(game_of_life_parallel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
target_link_libraries(game_of_life_parallel PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(game_of_life_parallel PUBLIC Threads::Threads)
target_compile_options(game_of_life_parallel PUBLIC ${OpenMP_CXX_FLAGS})
target_compile_options(game_of_life_parallel PUBLIC -DPARALLEL)


add_executable(game_of_life_serial ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                                   ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Board.cpp
                                   ${CMAKE_CURRENT_SOURCE_DIR}/src/core/BoardSerializer.cpp)
target_include_directories(game_of_life_serial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
target_link_libraries(game_of_life_serial PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(game_of_life_serial PUBLIC Threads::Threads)
target_compile_options(game_of_life_serial PUBLIC ${OpenMP_CXX_FLAGS})
target_compile_options(game_of_life_serial PUBLIC -DSERIAL)
