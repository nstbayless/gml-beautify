cmake_minimum_required(VERSION 2.0)
project (GMLBeautify)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(pugixml)
include_directories(.)

file(GLOB _SRC
    "*.cpp"
    "project/*.cpp"
    "project/resource/*.cpp"
)

file(GLOB _PUGIXML
  "pugixml/*.cpp"
)

add_executable(GMLBeautify
  ${_SRC}
  ${_PUGIXML}
)
