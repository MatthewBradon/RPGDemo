cmake_minimum_required(VERSION 3.15)
project(RPGDemo) 
set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


# Add source files
add_executable(RPGDemo src/main.cpp)


# Link libraries
target_link_libraries(RPGDemo PRIVATE 
    glad::glad 
    glfw 
    imgui::imgui
)