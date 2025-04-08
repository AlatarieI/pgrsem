cmake_minimum_required(VERSION 3.22)
project(noframework)

set(CMAKE_CXX_STANDARD 20)

# Add glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Add stb_image
add_library(stb_image STATIC lib/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC lib/stb_image)

# GLFW - If using a precompiled library
find_library(GLFW_LIB glfw3 PATHS lib/glfw3/lib-vc2022)
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

# Create executable
add_executable(noframework main.cpp
        GameObject.cpp
        GameObject.h
        Camera.cpp
        Camera.h)

# Copy shaders and resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader.vert DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fragment_shader.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Link libraries
target_link_libraries(noframework PRIVATE ${GLFW_LIB} glad OpenGL::GL stb_image glm)
