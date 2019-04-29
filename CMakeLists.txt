cmake_minimum_required(VERSION 3.7)
project(detector)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )


# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")



set(detector_srcs
	main.cpp
	detector.cpp
	detector.hpp
	communicator.cpp
	communicator.hpp
	routines.hpp
	routines.cpp
)
source_group("Detector" FILES ${detector_srcs})

add_executable(detector ${detector_srcs})
target_include_directories(detector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries( detector ${OpenCV_LIBS} )
