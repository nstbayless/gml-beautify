cmake_minimum_required(VERSION 2.0)
project (GMLBeautify)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB _SRC
    "*.cpp"
)

add_executable(GMLBeautify ${_SRC})
