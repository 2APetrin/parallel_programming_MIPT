cmake_minimum_required(VERSION 3.8)

project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "clang++")


set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(evaluator ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/integral_evaluator.cpp)
target_include_directories(evaluator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(evaluator PRIVATE pthread)
