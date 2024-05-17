cmake_minimum_required(VERSION 3.29)

project(GaijinTestTask_Server LANGUAGES CXX)

file(GLOB_RECURSE server_src_files "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(server ${server_src_files})

target_include_directories(server PUBLIC "${PROJECT_SOURCE_DIR}/include/")

find_package(Boost REQUIRED system)
target_link_libraries(server ${Boost_LIBRARIES} pthread)

