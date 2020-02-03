cmake_minimum_required(VERSION 3.14)
project(Container)

set(CMAKE_CXX_STANDARD 17)

set(CONTAINER_HDR DataContainer.h BinFile.h BinContainer.h)
set(CONTAINER_SRC DataContainer.cpp BinFile.cpp BinContainer.cpp)

get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)
if(IS_SUBPROJECT)
    set(CLONE_DIR ../Container)
else()
    set(CLONE_DIR ..)
endif()

add_library(Container Property ${CONTAINER_HDR} ${CONTAINER_SRC})
if(NOT EXISTS Property)
    exec_program("git" "" ARGS "clone https://github.com/nilrum/Property ${CLONE_DIR}/Property")
    message("Property cloned")
endif()
include_directories(Property/)
add_subdirectory(Property)