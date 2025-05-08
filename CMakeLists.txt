cmake_minimum_required(VERSION 3.30)
project(fileSystem)

set(CMAKE_CXX_STANDARD 20)

add_executable(fileSystem main.cpp
        RefCountedFile.cpp
)
