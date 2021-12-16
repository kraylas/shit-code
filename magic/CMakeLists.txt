cmake_minimum_required(VERSION 3.12.0)

project(main)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
find_package(fmt)
find_package(UTemplate)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
include_directories(inc)
link_directories(lib)
add_executable(y_comb_test y_combinator.cpp)
add_compile_definitions(USE_FMT)
target_link_libraries(y_comb_test
fmt::fmt
)

