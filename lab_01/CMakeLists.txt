cmake_minimum_required(VERSION 3.0)

project(lab_01)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab_01 lab_01.cpp)
target_link_libraries(lab_01 ${OpenCV_LIBS})
