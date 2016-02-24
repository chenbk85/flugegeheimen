cmake_minimum_required(VERSION 2.6)

set (FLUG_SOURCES src/main.cpp src/kernel/Kernel.cpp src/helpers/socket.cpp src/helpers/xml.cpp
        src/kernel/request_builder.cpp src/kernel/response_parser.cpp src/kernel/pool.cpp src/kernel/PollingBuffer.cpp
        src/devices/DummyDevice.cpp src/kernel/RemoteDispatcher.cpp src/kernel/DeviceManager.cpp src/kernel/Module.cpp
        src/kernel/DeviceDriver.cpp src/kernel/RequestEnumerator.cpp src/devices/AgilentOscope.cpp
        src/kernel/MonitorModule.cpp src/devices/NskSlowAdc.cpp src/kernel/BinBuilder.cpp src/kernel/Configurable.cpp
        src/devices/NskTimer.cpp src/devices/NskFastAdc.cpp src/kernel/DeviceHypervisor.cpp
        src/kernel/ArchiveModule.cpp src/kernel/JsonBson.cpp src/kernel/JsonBson.h src/kernel/MongoClient.cpp
        src/kernel/MongoClient.h src/kernel/ArchiveBackend.cpp src/kernel/ArchiveBackend.h
        src/kernel/DeviceProxy.cpp src/kernel/DeviceProxy.h src/kernel/DeviceCache.cpp src/kernel/DeviceCache.h
        src/kernel/Intercom.cpp src/kernel/Intercom.h src/devices/NskCrate.cpp src/devices/NskCrate.h
        src/kernel/LocalDispatcher.cpp src/kernel/LocalDispatcher.h src/devices/DummyCrate.cpp src/devices/DummyCrate.h
        src/kernel/PythonModule.cpp src/kernel/PythonModule.h src/kernel/MulticastBuilder.cpp src/kernel/MulticastBuilder.h src/kernel/ScriptModule.cpp src/kernel/ScriptModule.h src/kernel/SchedulerModule.cpp src/kernel/SchedulerModule.h src/kernel/PythonContext.cpp src/kernel/PythonContext.h src/kernel/ModulesTable.cpp src/kernel/ModulesTable.h)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

list( APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs -pthread")

find_package(Boost COMPONENTS thread filesystem system program_options regex python REQUIRED)

ADD_SUBDIRECTORY (gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}  /usr/include/python2.7
        ${Boost_INCLUDE_DIR} /usr/include/jsoncpp/ /usr/include/mongo-client /usr/include/glib-2.0
        /usr/local/include/mongo)

add_executable (flugegeheimen ${FLUG_SOURCES})

target_link_libraries(flugegeheimen -L/usr/local/lib/ -ljsoncpp -lmongo-client -lmongoclient -lpython2.7 ${Boost_LIBRARIES}
        ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_PYTHON_LIBRARY} )

add_executable(flugegeheimen-unit-test src/test.cpp src/helpers/xml.cpp src/helpers/xml_test.cpp src/helpers/socket.cpp
        src/helpers/socket_test.cpp src/helpers/binres.cpp src/helpers/binres_test.cpp src/kernel/pool.cpp
        src/kernel/pool_test.cpp src/kernel/PollingBuffer.cpp)

target_link_libraries(flugegeheimen-unit-test gtest gtest_main -L/usr/local/lib/
        -ljsoncpp -lmongo-client -lmongoclient -lpython2.7
        ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY}
        ${Boost_REGEX_LIBRARY} ${Boost_PYTHON_LIBRARY})

add_test( flugegeheimen-unit-test flugegeheimen-unit-test )
