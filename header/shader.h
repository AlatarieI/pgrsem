//----------------------------------------------------------------------------------------
/**
 * \file       shader.h
 * \author     skorelu1
 * \date       2025/04/26
 * \brief      OpenGL shader program management class
 *
 *  Handles shader compilation, linking and activation. Manages both vertex
 *  and fragment shader sources with error checking during program creation.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include "utility.h"

/// OpenGL shader program wrapper
class Shader {
public:
    GLuint id;                      ///< OpenGL program object ID
    std::string vertexShaderSource; ///< Vertex shader GLSL source code
    std::string fragmentShaderSource; ///< Fragment shader GLSL source code

    /// Constructor with shader sources
    /**
     * \param[in] vertexShaderSource    Path to vertex shader source file
     * \param[in] fragmentShaderSource  Path to fragment shader source file
     */
    Shader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    /// Compile and link shader program
    /**
     * Compiles attached shaders and links them into a complete program.
     * Performs error checking for compilation and linking errors.
     */
    void createProgram();

    /// Activate the shader program
    /**
     * Makes this shader program current for subsequent rendering calls.
     * Equivalent to glUseProgram() with this program's ID.
     */
    void use();
};

#endif // SHADER_H