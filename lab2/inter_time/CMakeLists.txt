cmake_minimum_required(VERSION 3.8)

project(lab2 LANGUAGES C CXX)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_C_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(pipe ${CMAKE_CURRENT_SOURCE_DIR}/src/pipe.cc)
target_link_libraries(pipe PRIVATE pthread)

add_executable(shared ${CMAKE_CURRENT_SOURCE_DIR}/src/shared.c)
target_link_libraries(shared PRIVATE pthread)
