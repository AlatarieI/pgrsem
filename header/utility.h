//----------------------------------------------------------------------------------------
/**
 * \file       utility.h
 * \author     skorelu1
 * \date       2025/04/10
 * \brief      Utility functions for OpenGL resource loading
 *
 *  Provides helper functions for loading textures, shader files, and cubemaps.
 *  Handles common I/O operations and OpenGL resource creation.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef UTILITY_H
#define UTILITY_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

/// Load 2D texture from image file
/**
 * \param[in] path  Path to texture image file (JPEG, PNG, etc.)
 * \return OpenGL texture ID
 */
GLuint load_texture(char const * path);

/// Read text file contents into string
/**
 * \param[in] filename  Path to text file (shaders, configs, etc.)
 * \return File contents as std::string
 */
std::string read_file(const char* filename);

/// Load cube map texture from directory
/**
 * Loads 6 cube map faces from directory with standard naming convention:
 * - nx.png
 * - px.png
 * - nz.png
 * - pz.png
 * - ny.png
 * - py.png

 * \param[in] dir  Directory containing cube map face textures
 * \return OpenGL cube map texture ID
 */
GLuint load_cube_map(std::string dir);

#endif // UTILITY_H