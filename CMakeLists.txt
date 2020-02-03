cmake_minimum_required(VERSION 3.14)
project(Container)

set(CMAKE_CXX_STANDARD 17)

set(CONTAINER_HDR DataContainer.h BinFile.h BinContainer.h)
set(CONTAINER_SRC DataContainer.cpp BinFile.cpp BinContainer.cpp)
set(IS_OTH_GTEST OFF)

add_library(Container Property ${CONTAINER_HDR} ${CONTAINER_SRC})
if(NOT EXISTS "Property/")
    exec_program("git" "" ARGS "clone https://github.com/nilrum/Property ../Property")
    message("Property cloned")
endif()
include_directories(Property/)
add_subdirectory(Property)