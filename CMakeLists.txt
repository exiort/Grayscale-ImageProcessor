cmake_minimum_required(VERSION 3.12)
project(GrayscaleImage)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Define source files
set(SOURCES
    src/Image.cpp
    app/main.cpp
)

# Define header files
set(HEADERS
    include/Image.hpp
)

# Create an executable target
add_executable(GrayscaleImage ${SOURCES} ${HEADERS})

# Add the include directory to the project
target_include_directories(GrayscaleImage PRIVATE include)

# Set the CXX flags to enable debugging information
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
