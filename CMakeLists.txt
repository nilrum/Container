cmake_minimum_required(VERSION 3.14)
project(Container)

set(CMAKE_CXX_STANDARD 17)

set(CONTAINER_HDR DataContainer.h BinFile.h BinContainer.h)
set(CONTAINER_SRC DataContainer.cpp BinFile.cpp BinContainer.cpp)

if(NOT TARGET Container)
    if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Property)
        exec_program("git" "" ARGS "clone https://github.com/nilrum/Property ${CMAKE_CURRENT_SOURCE_DIR}/Property")
        message("Property cloned")
    endif()

    add_subdirectory(Property)

    add_library(Container ${CONTAINER_HDR} ${CONTAINER_SRC})
    target_include_directories(Container PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC Property)
    target_link_libraries(Container Property)
endif()