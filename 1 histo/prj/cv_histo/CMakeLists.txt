cmake_minimum_required(VERSION 3.0)

project(cv_histo)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_histo cv_histo.cpp)
target_link_libraries(cv_histo ${OpenCV_LIBS})