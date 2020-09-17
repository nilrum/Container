cmake_minimum_required(VERSION 3.12)

project(ContainerTest)

set(CONTAINER_FILES ${CONTAINER_HDR} ${CONTAINER_SRC})

#Add include files
foreach(F ${CONTAINER_FILES})
    set(INCL_CONTAINER ${INCL_CONTAINER} ../${F})
endforeach()

#Add test files
set(CONTAINER_TESTS DataContainerTest.cpp MID4ContainerTest.cpp)

#Target
if(NOT TARGET ContainerTest)
    add_executable(ContainerTest ${CONTAINER_TESTS} ${INCL_CONTAINER})
    target_include_directories(ContainerTest PUBLIC .. ${GOOGLETEST_DIR}/include)
    target_link_libraries(ContainerTest Container gtest gtest_main gmock)
endif()