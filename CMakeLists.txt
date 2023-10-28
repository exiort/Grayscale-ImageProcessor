cmake_minimum_required(VERSION 3.12)
project(GrayscaleImage)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/Image.cpp
    app/main.cpp
)

set(HEADERS
    include/Image.hpp
)

add_executable(GrayscaleImage ${SOURCES} ${HEADERS})

target_include_directories(GrayscaleImage PRIVATE include)
