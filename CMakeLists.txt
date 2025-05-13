cmake_minimum_required(VERSION 3.22)
project(pgrsem)

set(CMAKE_CXX_STANDARD 20)

# Create executable
add_executable(pgrsem
        main.cpp
        src/camera.cpp
        src/mesh.cpp
        src/model.cpp
        src/utility.cpp
        src/scene.cpp
        src/scene_object.cpp
        src/lights.cpp
        src/shader.cpp
        src/sky_box.cpp
)


target_include_directories(pgrsem PRIVATE header)

target_include_directories(pgrsem PRIVATE lib)


# Add ImGui sources
set(IMGUI_SRC
        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC imgui lib/imgui/backends)
target_include_directories(imgui PUBLIC lib/imgui)

# Add glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

# Add stb_image
add_library(stb_image STATIC lib/stb_image/stb_image.cpp
        lib/stb_image/stb_image_write.h)
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
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/lib/assimp")

# Full path to the .lib file (adjust name if it's different)
set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/x64/assimp-vc143-mt.lib")

# Include directory
include_directories("${ASSIMP_DIR}/include")


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
#add_custom_command(
#        TARGET pgrsem POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/lib
#        ${CMAKE_BINARY_DIR}/lib
#        COMMENT "Copying lib folder to build directory"
#)

add_custom_command(TARGET pgrsem POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASSIMP_DIR}/bin/x64/assimp-vc143-mt.dll"
        $<TARGET_FILE_DIR:pgrsem>)




# Link libraries
target_link_libraries(pgrsem PRIVATE ${GLFW_LIB} glad OpenGL::GL stb_image glm ${ASSIMP_LIBRARY} imgui)
