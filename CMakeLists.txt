cmake_minimum_required(VERSION 3.22)
project(pgrsem)

set(CMAKE_CXX_STANDARD 20)

# Add glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Add stb_image
add_library(stb_image STATIC lib/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC lib/stb_image)

# GLFW - If using a precompiled library
find_library(GLFW_LIB glfw3 PATHS lib/glfw3/lib-vc2022)
#find_library(GLFW_LIB glfw3 PATHS lib/glfw3/lib-mingw-w64)
if(NOT GLFW_LIB)
    message(FATAL_ERROR "GLFW not found in lib/glfw3/lib-vc2022")
endif()
include_directories(lib/glfw3/include)

# GLM - If it's a local dependency
if(EXISTS "${CMAKE_SOURCE_DIR}/lib/glm/CMakeLists.txt")
    add_subdirectory(lib/glm)
else()
    find_package(glm REQUIRED)
endif()
include_directories(lib/glm)

# OpenGL
find_package(OpenGL REQUIRED)

#Assimp
# Assimp - Manual linking
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/lib/Assimp")
# Full path to the .lib file (adjust name if it's different)
set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/x64/assimp-vc143-mt.lib")
# Include directory
include_directories("${ASSIMP_DIR}/include")

# Create executable
add_executable(pgrsem main.cpp
        GameObject.cpp
        GameObject.h
        Camera.cpp
        Camera.h)

# Copy shaders and resources
#file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(
        TARGET pgrsem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Copying shader folder to build directory"
)
add_custom_command(
        TARGET pgrsem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
        COMMENT "Copying resource folder to build directory"
)


# Link libraries
target_link_libraries(pgrsem PRIVATE ${GLFW_LIB} glad OpenGL::GL stb_image glm ${ASSIMP_LIBRARY})
