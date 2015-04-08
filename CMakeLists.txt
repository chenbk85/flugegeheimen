cmake_minimum_required(VERSION 2.6)


set (FLUG_SOURCES src/main.cpp src/kernel/kernel.cpp)

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")

ADD_SUBDIRECTORY (gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable (flugegeheimen ${FLUG_SOURCES})
add_executable(flugegeheimen-unit-test src/test.cpp src/helpers/xml.cpp src/helpers/xml_test.cpp)


target_link_libraries(flugegeheimen-unit-test gtest gtest_main)
add_test( flugegeheimen-unit-test flugegeheimen-unit-test )
