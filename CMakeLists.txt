cmake_minimum_required(VERSION 3.30)
project(MyThreadPool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyThreadPool
        线程池项目测试.cpp
        threadpool.cpp
        threadpool.h
)