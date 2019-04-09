cmake_minimum_required(VERSION 3.0)

project(cv_histo_slider)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_histo_slider cv_histo_slider.cpp)
target_link_libraries(cv_histo_slider ${OpenCV_LIBS})