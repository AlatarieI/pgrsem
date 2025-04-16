//
// Created by luky9 on 10.04.2025.
//

#ifndef UTILITY_H
#define UTILITY_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include <sstream>

GLuint load_texture(char const * path);
std::string readFile(const char* filename);

#endif //UTILITY_H
