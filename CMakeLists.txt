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

# 4. Core library sources (always required)
set(CORE_SOURCES
    src/utils.cpp
    src/xml_parser.cpp
)

# 5. Optional: sorting module — included only if the file exists.
#    Removing xml_sorter.cpp / xml_sorter.h will NOT break the core library.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/xml_sorter.cpp")
    list(APPEND CORE_SOURCES src/xml_sorter.cpp)
    add_compile_definitions(XML_SORTER_AVAILABLE)
    message(STATUS "xml_sorter module found — sorting feature enabled")
else()
    message(STATUS "xml_sorter module not found — sorting feature disabled")
endif()

# 6. Optional: search module — included only if the file exists.
#    Removing xml_searcher.cpp / xml_searcher.h will NOT break the core library.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/xml_searcher.cpp")
    list(APPEND CORE_SOURCES src/xml_searcher.cpp)
    add_compile_definitions(XML_SEARCHER_AVAILABLE)
    message(STATUS "xml_searcher module found — search feature enabled")
else()
    message(STATUS "xml_searcher module not found — search feature disabled")
endif()

add_library(xml_parsing ${CORE_SOURCES})

option(BUILD_SAMPLES "Build sample applications" ON)

if(BUILD_SAMPLES)
    add_executable(my_app 
        samples/main.cpp
    )
    target_link_libraries(my_app xml_parsing)
endif()
