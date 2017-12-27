cmake_minimum_required(VERSION 3.9)
project(inter)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. SRCS)
add_executable(RangeScanOO ${SRCS})
target_link_libraries(libglut)
