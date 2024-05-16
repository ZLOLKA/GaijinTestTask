cmake_minimum_required(VERSION 3.29)

project(GaijinTestTask_Client LANGUAGES CXX)

file(GLOB_RECURSE client_src_files "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(client ${client_src_files})

target_include_directories(client PUBLIC "${PROJECT_SOURCE_DIR}/include/")

