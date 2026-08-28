cmake_minimum_required(VERSION 3.28.3)
project(CDaemon)

add_executable(cdaemon src/main.c src/tuya_handle.c)

add_subdirectory(external/tuya-iot-core-sdk)
target_include_directories(cdaemon PRIVATE external/include include)
target_link_libraries(cdaemon PRIVATE link_core)