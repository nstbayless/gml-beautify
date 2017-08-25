cmake_minimum_required(VERSION 2.0)
project (GMLBeautify)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB _SRC
    "*.cpp"
)

add_executable(GMLBeautify ${_SRC})
