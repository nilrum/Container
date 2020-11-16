cmake_minimum_required(VERSION 3.12)
project(Container)

set(CMAKE_CXX_STANDARD 17)

set(CONTAINER_HDR
        DataContainer.h
        Units.h
        SelectLoader.h
        LasContainer.h
        Las.h
        BinFile.h
        BinContainer.h
        Bin/MID4.h
        Bin/MIDK04.h
        Bin/MIDK05.h
        )
set(CONTAINER_SRC
        DataContainer.cpp
        Units.cpp
        SelectLoader.cpp
        LasContainer.cpp
        Las.cpp
        BinContainer.cpp
        BinFile.cpp
        Bin/MID4.cpp
        Bin/MIDK04.cpp
        Bin/MIDK05.cpp
        )
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if(NOT TARGET Property)
    if(NOT EXISTS ${PROPERTY_DIR})
        exec_program("git" "" ARGS "clone https://github.com/nilrum/Property ${CMAKE_CURRENT_SOURCE_DIR}/Property")
        message("Property cloned")
        set(PROPERTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Property CACHE FILEPATH "PROPERTY_DIR")
        message("Added PROPERTY_DIR = [${PROPERTY_DIR}]")
    endif()
    add_subdirectory(Property)
endif()

if(NOT TARGET Container)
    add_library(Container ${CONTAINER_HDR} ${CONTAINER_SRC})
    target_include_directories(Container PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${PROPERTY_DIR})
    target_link_libraries(Container Property)
endif()

#Если gtest не подключен
if(NOT TARGET gtest)
    if(NOT EXISTS ${GOOGLETEST_DIR})
        exec_program("git" "" ARGS "clone --depth=1 https://github.com/google/googletest ${CMAKE_CURRENT_SOURCE_DIR}/googletest")
        message("Googletest cloned")
        set(GOOGLETEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest CACHE FILEPATH "GOOGLETEST_DIR")
        message("Added GOOGLETEST_DIR = [${GOOGLETEST_DIR}]")
    endif()
    add_subdirectory(googletest)
endif()

add_subdirectory(tests)