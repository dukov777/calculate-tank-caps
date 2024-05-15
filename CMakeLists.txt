cmake_minimum_required(VERSION 3.10)
project(MyJsonApp)

set(CMAKE_CXX_STANDARD 17)

# create list with source files
set(SOURCE_FILES
    main.cpp 
    src/TankCalculator.cpp
    src/Capacitors.cpp
    src/MonitorDecorator.cpp
)
# compile the source files
add_executable(calculate-tank-caps ${SOURCE_FILES})

# Include the JSON library
include_directories(${CMAKE_SOURCE_DIR}/argparse/include/)
include_directories(${CMAKE_SOURCE_DIR}/json/include/)
include_directories(${CMAKE_SOURCE_DIR}/include/)
