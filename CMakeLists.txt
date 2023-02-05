cmake_minimum_required(VERSION 3.24)
project(mix_tinyrenderer_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(mix_tinyrenderer_c main.c model.h model.c)
target_link_libraries(${PROJECT_NAME} m cglm_headers)

add_subdirectory(cglm EXCLUDE_FROM_ALL)