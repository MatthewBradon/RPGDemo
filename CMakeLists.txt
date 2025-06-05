cmake_minimum_required(VERSION 3.15)
project(RPGDemo) 
set(CMAKE_CXX_STANDARD 17)

# Find Packages
find_package(fmt CONFIG REQUIRED)


# Add source files
add_executable(RPGDemo main.cpp)


# Link libraries
target_link_libraries(RPGDemo PRIVATE fmt::fmt)