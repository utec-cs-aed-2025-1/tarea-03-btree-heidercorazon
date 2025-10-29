cmake_minimum_required(VERSION 3.29)
project(tarea_03_btree_heidercorazon)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(tarea_03_btree_heidercorazon
    btree.h
    main.cpp
    node.h
    tester.h)
