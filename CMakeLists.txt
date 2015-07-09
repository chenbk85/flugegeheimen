cmake_minimum_required(VERSION 2.6)

set (FLUG_SOURCES src/main.cpp src/kernel/kernel.cpp src/helpers/socket.cpp src/helpers/xml.cpp
src/kernel/request_builder.cpp src/kernel/response_parser.cpp
src/devices/timer.cpp src/devices/adc.cpp src/helpers/binres.cpp
src/kernel/pool.cpp)
#src/helpers/csv.cpp)

list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs -pthread")

ADD_SUBDIRECTORY (gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable (flugegeheimen ${FLUG_SOURCES})
add_executable(flugegeheimen-unit-test src/test.cpp
src/helpers/xml.cpp src/helpers/xml_test.cpp
src/helpers/socket.cpp src/helpers/socket_test.cpp
src/helpers/binres.cpp src/helpers/binres_test.cpp
src/kernel/pool.cpp src/kernel/pool_test.cpp)
#src/helpers/csv.cpp src/helpers/csv_test.cpp)

target_link_libraries(flugegeheimen-unit-test gtest gtest_main)
add_test( flugegeheimen-unit-test flugegeheimen-unit-test )
