cmake_minimum_required(VERSION 2.0)
project (GMLBeautify)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB _SRC
    "*.cpp"
    "project/*.cpp"
    "project/resource/*.cpp"
)

include_directories(pugixml)
include_directories(catch)
include_directories(.)

file(GLOB _PUGIXML
  "pugixml/*.cpp"
)

file(GLOB _TESTS
  "tests/*.cpp"
)

add_executable(gml-tools "main/main.cpp")
add_executable(gml-tools-test ${_TESTS})

add_library(GMLTools
  ${_SRC}
  ${_PUGIXML}
)

target_link_libraries(gml-tools GMLTools)
target_link_libraries(gml-tools-test GMLTools)