cmake_minimum_required(VERSION 3.22)
project(noframework)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib/glfw3/include)
link_directories(lib/glfw3/lib-vc2022)

add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

add_library(stb_image STATIC lib/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC lib/stb_image)

# Find OpenGL
find_package(OpenGL REQUIRED)


add_executable(noframework main.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vertex_shader.vert ${CMAKE_CURRENT_BINARY_DIR}/vertex_shader.vert COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fragment_shader.frag ${CMAKE_CURRENT_BINARY_DIR}/fragment_shader.frag COPYONLY)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})


# Link libraries
target_link_libraries(noframework glfw3 glad OpenGL::GL stb_image)