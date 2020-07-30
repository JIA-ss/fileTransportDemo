cmake_minimum_required(VERSION 2.8)
project(ClientExe)
add_definitions(-std=c++11)
add_definitions(-static-libgcc -static-libstdc++)

aux_source_directory(${PROJECT_SOURCE_DIR} SRCS)
add_executable(ClientDemo client.cpp ${SRCS})
#
