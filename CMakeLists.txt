cmake_minimum_required(VERSION 3.14)

project(lubgen_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lubgen STATIC
    src/table.cpp
    src/csv_reader.cpp
    src/query_engine.cpp
)
target_include_directories(lubgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(lubgen_db src/main.cpp)
target_link_libraries(lubgen_db PRIVATE lubgen)

