cmake_minimum_required(VERSION 3.26)
project(Project3)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project3 src/main.cpp
        src/Play.h
        src/Play.cpp
        src/ComparePlay.h
        src/ComparePlay.cpp
        src/PlayMaxHeap.cpp
        src/PlayMaxHeap.h
        src/Helpers.h
        src/Helpers.cpp
        src/PlayHashTable.cpp
        src/PlayHashTable.h
        src/NodePlay.h
        src/NodePlay.cpp)