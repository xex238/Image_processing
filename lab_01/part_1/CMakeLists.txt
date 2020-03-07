cmake_minimum_required(VERSION 3.0)

project(lab_01_part_1)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab_01_part_1 lab_01_part_1.cpp)
target_link_libraries(lab_01_part_1 ${OpenCV_LIBS})