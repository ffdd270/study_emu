cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 14)

include_directories(
		header
		lib/imgui
)

add_library(harucar
		src/library.cpp
		header/library.h
		header/CPUViewer.h
		header/CPUProvider.h
		header/Viewer.h
		header/Provider.h
		src/CPUViewer.cpp
		src/CPUProvider.cpp)