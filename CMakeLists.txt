cmake_minimum_required(VERSION 3.10)
project(MyJsonApp)

set(CMAKE_CXX_STANDARD 17)

# create list with source files
set(SOURCE_FILES
    main.cpp 
    src/TankCalculator.cpp
    src/Capacitors.cpp
    src/MonitorDecorator.cpp
    src/MemoryMngr.cpp
)

# Add -fsanitize=address flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")


# compile the source files
add_executable(calculate-tank-caps ${SOURCE_FILES})

target_compile_options(calculate-tank-caps PRIVATE -DMEMORYPOOL)

# Include the JSON library
include_directories(${CMAKE_SOURCE_DIR}/argparse/include/)
include_directories(${CMAKE_SOURCE_DIR}/json/include/)
include_directories(${CMAKE_SOURCE_DIR}/include/)
