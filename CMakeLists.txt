cmake_minimum_required(VERSION 3.28.3)
project(TuyaD)

add_executable(tuyad src/main.c src/tuya_handle.c src/arg_parse.c src/log_wrapper.c src/net_interface_parser.c src/cpu_monitoring.c src/daemon.c src/json_helper.c)

add_subdirectory(external/tuya-iot-core-sdk)
target_include_directories(tuyad PRIVATE external/include include)
target_link_libraries(tuyad PRIVATE link_core)