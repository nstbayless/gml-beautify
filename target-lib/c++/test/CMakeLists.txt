cmake_minimum_required(VERSION 2.0)
project (OGMTest)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../include)

file(GLOB _SRC
    "*.cpp"
    "../src/*.cpp"
    "../src/function/*.cpp"
)

add_executable(OGMTest
  ${_SRC}
)
