cmake_minimum_required(VERSION 3.0)

project(cw_01)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cw_01 cw_01.cpp)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Cutie_cat.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(cw_01 ${OpenCV_LIBS})
