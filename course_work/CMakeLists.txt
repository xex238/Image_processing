cmake_minimum_required(VERSION 3.0)

project(course_work)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(course_work course_work.cpp)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Cutie_cat.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tid2013 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/MIDV-500 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(course_work ${OpenCV_LIBS})
