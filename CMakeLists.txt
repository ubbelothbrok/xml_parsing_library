# 1. Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# 2. Define the project name, version, and language
project(xml_parsing 
        VERSION 1.0.0 
        LANGUAGES CXX)

# 3. Enforce the C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 4. Define the executable target and its source files

add_library(xml_parsing 
    src/utils.cpp
    src/xml_parser.cpp
)

option(BUILD_SAMPLES "Build sample applications" ON)

if(BUILD_SAMPLES)
    add_executable(my_app 
        samples/main.cpp
    )
    target_link_libraries(my_app xml_parsing)
endif()
