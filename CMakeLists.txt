cmake_minimum_required(VERSION 3.7)
project(detector)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
SET(OpenCV_DIR /home/pi/opencv/installation/OpenCV-master/lib/cmake/opencv4)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")



set(detector_srcs
	main.cpp
)
source_group("Detector" FILES ${detector_srcs})

add_executable(detector ${detector_srcs})
target_include_directories(detector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})



