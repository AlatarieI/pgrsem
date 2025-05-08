//----------------------------------------------------------------------------------------
/**
 * \file       sky_box.h
 * \author     skorelu1
 * \date       2025/04/26
 * \brief      Skybox rendering class using cube mapping
 *
 *  Implements a skybox/cube map environment with support for multiple texture blending.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef SKY_BOX_H
#define SKY_BOX_H

#include <gtc/type_ptr.hpp>
#include "camera.h"
#include "model.h"
#include "utility.h"

/// Skybox rendering class
class SkyBox {
public:
    Mesh* mesh;                 ///< Cube mesh for skybox rendering
    int shaderIdx;              ///< Index of associated shader program
    std::vector<Texture> textures; ///< Cube map textures
    bool shouldBlend;           ///< Flag for texture blending
    GLuint VAO;                 ///< Vertex Array Object
    GLuint VBO;                 ///< Vertex Buffer Object
    GLuint EBO;                 ///< Element Buffer Object
    int size;                   ///< Texture resolution (assumes square textures)
    int currTextureIdx = 0;     ///< Index of current active texture for blending

    /// Constructor with single cubemap texture directory
    /**
     * Loads cubemap textures from specified directory path. Assumes textures
     * are named according to standard cubemap conventions.
     *
     * \param[in] texturePath  Path to directory containing cubemap textures
     * \param[in] shaderIdx    Index of skybox shader in scene's shader array
     */
    SkyBox(std::string texturePath, int shaderIdx);

    /// Constructor with explicit texture paths
    /**
     * \param[in] texturePaths  Array of texture paths in order:
     *                          [right, left, top, bottom, front, back]
     * \param[in] shaderIdx     Index of skybox shader in scene's shader array
     */
    SkyBox(const std::vector<std::string>& texturePaths, int shaderIdx);

    /// Render the skybox
    /**
     * \param[in] shader          OpenGL shader program ID
     * \param[in] currentCamera   Reference to active scene camera
     * \param[in] projection      Camera projection matrix
     * \param[in] blend           Texture blending factor (0-1)
     */
    void draw(GLuint shader, Camera& currentCamera, glm::mat4 projection, float blend);

private:
    /// Initialize OpenGL buffers and cube geometry
    void setupSkyBox();
};

#endif // SKY_BOX_H