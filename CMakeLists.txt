cmake_minimum_required(VERSION 3.10)
project(MyJsonApp)

set(CMAKE_CXX_STANDARD 17)

add_executable(calculate-tank-caps main.cpp)

# Include the JSON library
include_directories(${CMAKE_SOURCE_DIR}/argparse/include/)
