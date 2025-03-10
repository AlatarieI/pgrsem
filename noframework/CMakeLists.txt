cmake_minimum_required(VERSION 3.22)
project(noframework)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib/glfw3/include)
link_directories(lib/glfw3/lib-vc2022)

add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Find OpenGL
find_package(OpenGL REQUIRED)


add_executable(noframework main.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader.vert ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.vert COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fragment_shader.frag ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.frag COPYONLY)


# Link libraries
target_link_libraries(noframework glfw3 glad OpenGL::GL)