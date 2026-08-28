cmake_minimum_required(VERSION 3.28.3)
project(CDaemon)

add_executable(cdaemon ./src/main.c)
target_link_libraries(link_core)