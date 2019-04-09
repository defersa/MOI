cmake_minimum_required(VERSION 3.0)

project(cv_idz)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_idz cv_idz.cpp)
target_link_libraries(cv_idz ${OpenCV_LIBS})