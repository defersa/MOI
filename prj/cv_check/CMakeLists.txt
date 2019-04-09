cmake_minimum_required(VERSION 3.0)

project(cv_check)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_check cv_check.cpp)
target_link_libraries(cv_check ${OpenCV_LIBS})