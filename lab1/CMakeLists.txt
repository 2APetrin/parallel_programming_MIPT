cmake_minimum_required(VERSION 3.8)


project(lab1 LANGUAGES C)

set(CMAKE_C_COMPILER "mpicc")

set(CMAKE_C_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


add_executable(pi_eval ${CMAKE_CURRENT_SOURCE_DIR}/src/pi_eval/pi_eval.c ${CMAKE_CURRENT_SOURCE_DIR}/src/pi_eval/eval.c)

target_include_directories(pi_eval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pi_eval)
target_link_libraries(pi_eval PRIVATE m)


add_executable(diffusion ${CMAKE_CURRENT_SOURCE_DIR}/src/diffusion/diffusion.c 
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/diffusion/evaluation.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/diffusion/run_info.c)
target_include_directories(diffusion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/diffusion)
target_link_libraries(diffusion PRIVATE m)


add_executable(timer ${CMAKE_CURRENT_SOURCE_DIR}/src/interaction_time/inter_time.c)
# target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/interraction_time)

# target_include_directories(pi_eval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pi_eval)
# target_link_libraries(pi_eval PRIVATE m)
