cmake_minimum_required(VERSION 3.15)
project(RPGDemo)
set(CMAKE_CXX_STANDARD 17)

# Packages
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GLUT REQUIRED)
find_package(Stb REQUIRED)

# Create timestamped DLL name
string(TIMESTAMP CURRENT_TIMESTAMP "%Y%m%d%H%M%S" UTC)
set(TIMESTAMPED_DLL_NAME "game_${CURRENT_TIMESTAMP}")

# Game library target (timestamped DLL)
add_library(${TIMESTAMPED_DLL_NAME} SHARED
    src/game.cpp
)

# Set output directory
set_target_properties(${TIMESTAMPED_DLL_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Copy to game.dll after build, then delete timestamped version
add_custom_command(TARGET ${TIMESTAMPED_DLL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
            "$<TARGET_FILE:${TIMESTAMPED_DLL_NAME}>" 
            "${CMAKE_SOURCE_DIR}/game.dll"
    COMMAND ${CMAKE_COMMAND} -E remove 
            "$<TARGET_FILE:${TIMESTAMPED_DLL_NAME}>"
    COMMENT "Copying to game.dll and cleaning up"
)

# RPGDemo executable
add_executable(RPGDemo
    src/main.cpp
)


# Set output directory for all configurations
set_target_properties(RPGDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}"
)

# Include directories
target_include_directories(${TIMESTAMPED_DLL_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x64-windows/include
)
target_include_directories(RPGDemo PRIVATE
    ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x64-windows/include
)

# Link libraries
target_link_libraries(${TIMESTAMPED_DLL_NAME} PRIVATE
    glad::glad
    glfw
    imgui::imgui
    GLUT::GLUT
)
target_link_libraries(RPGDemo PRIVATE
    glad::glad
    glfw
    imgui::imgui
    GLUT::GLUT
)