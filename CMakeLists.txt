cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_GENERATOR Ninja)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_LINKER_TYPE LLD)

project(GaijinTestTask LANGUAGES CXX)

add_subdirectory(Server)

add_subdirectory(Client)

