cmake_minimum_required(VERSION 3.15)
project(RPGDemo) 
set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GLUT REQUIRED)
find_package(Stb REQUIRED)

# Add source files
add_executable(RPGDemo src/main.cpp)

target_include_directories(RPGDemo PRIVATE
    ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x64-windows/include
)

# Link libraries
target_link_libraries(RPGDemo PRIVATE 
    glad::glad 
    glfw 
    imgui::imgui
    GLUT::GLUT
)
set_target_properties(RPGDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}"
)