cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 14)

add_library(harucar
		src/library.cpp
		header/library.h)