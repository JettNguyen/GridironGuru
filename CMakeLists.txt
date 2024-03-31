cmake_minimum_required(VERSION 3.26)
project(Project3)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project3 main.cpp
        Play.h
        Play.cpp
        ComparePlay.h
        ComparePlay.cpp
        PlayMaxHeap.cpp
        PlayMaxHeap.h
        Helpers.h
        Helpers.cpp
        PlayHashTable.cpp
        PlayHashTable.h)
