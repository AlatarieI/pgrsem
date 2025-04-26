//
// Created by luky9 on 26.04.2025.
//

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>


#include "utility.h"

class Shader {
public:
    GLuint id;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    Shader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    void createProgram();
};



#endif //SHADER_H
