cmake_minimum_required(VERSION 3.10)
project(cmodel_example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
SET( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -std=c++11"   CACHE STRING "c++ flags" )
SET( CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}"   CACHE STRING "c flags" )


include_directories(${DDK_HOME}/include/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cmodel_test/include)
link_directories(${PROJECT_SOURCE_DIR}/cmodel_test/lib)
add_executable(cmodel_example main.cpp)

target_link_libraries(cmodel_example
        "-Wl,-Bdynamic"
        -lDvpp_api
        -lopencv_core
        -lopencv_imgproc
        -lopencv_imgcodecs
        )