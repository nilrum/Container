cmake_minimum_required(VERSION 3.14)

project(ContainerTest)

set(CONTAINER_FILES ${CONTAINER_HDR} ${CONTAINER_SRC})

#Add include files
foreach(F ${CONTAINER_FILES})
    set(INCL_CONTAINER ${INCL_CONTAINER} ../${F})
endforeach()

#Add include dirs
include_directories(.. GTEST_DIR GTEST_DIR/include)

#Add test files
set(CONTAINER_TESTS DataContainerTest.cpp)

#Target
if(NOT TARGET ContainerTest)
    add_executable(ContainerTest ${CONTAINER_TESTS} ${INCL_CONTAINER})
    target_link_libraries(ContainerTest PUBLIC Container gtest gtest_main gmock)
endif()