cmake_minimum_required(VERSION 3.29)

project(GaijinTestTask_Server LANGUAGES CXX)

file(GLOB_RECURSE server_src_files "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(server ${server_src_files})

target_include_directories(server PUBLIC "${PROJECT_SOURCE_DIR}/include/")

find_package(Boost 1.85.0 REQUIRED system)
target_link_libraries(server
    PUBLIC Boost::system
)
target_link_libraries(server
    PUBLIC pthread
    PUBLIC uring
)

target_include_directories(server PUBLIC ${Boost_INCLUDE_DIRS})
target_compile_definitions(server
    PUBLIC BOOST_ASIO_HAS_FILE
)
target_compile_definitions(server
    PUBLIC BOOST_ASIO_HAS_EPOLL
    PUBLIC BOOST_ASIO_HAS_IO_URING
)

