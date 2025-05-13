//----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     skorelu1
 * \date       2025/04/10
 * \brief      3D model loader and renderer using Assimp library
 *
 *  Handles loading of 3D models from various file formats, processes meshes,
 *  materials and textures.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "utility.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

/// 3D model class handling loading and rendering of complex objects
class Model {
public:
    std::vector<Texture> textures_loaded;  ///< Cache of loaded textures to avoid duplicates
    std::vector<Mesh> meshes;              ///< All meshes comprising the model
    std::string directory;                 ///< Base directory of the model file
    bool gammaCorrection;                  ///< Flag for SRGB color space correction
    bool flipUV;                           ///< Flag for vertical texture coordinate flipping
    glm::vec3 aabbMin;                     ///< Minimum for AABB for collision detection
    glm::vec3 aabbMax;                     ///< Maximum for AABB for collision detection

    /// Constructor from model file path
    /**
     * \param[in] path     Path to the model file (e.g .obj, .fbx)
     * \param[in] flipUV   Flip texture coordinates vertically (default: false)
     * \param[in] gamma    Enable gamma correction (default: false)
     */
    Model(std::string const &path, bool flipUV = false, bool gamma = false);

    /// Constructor from existing meshes
    /**
     * \param[in] meshes   Pre-constructed meshes to compose the model
     * \param[in] flipUV   Flip texture coordinates vertically (default: false)
     * \param[in] gamma    Enable gamma correction (default: false)
     */
    Model(std::vector<Mesh> meshes, bool flipUV = false, bool gamma = false);

    /// Render all meshes in the model
    /**
     * \param[in] program  OpenGL shader program ID for rendering
     */
    void draw(GLuint program);

private:
    /// Load model from file using Assimp
    /**
     * \param[in] path     Path to the model file
     * \param[in] flipUV   Vertical texture flip flag
     */
    void loadModel(std::string const &path, bool flipUV);

    /// Process Assimp node recursively
    /**
     * \param[in] node    Current node to process
     * \param[in] scene   Pointer to Assimp scene data
     */
    void processNode(aiNode *node, const aiScene *scene);

    /// Convert Assimp mesh to our Mesh structure
    /**
     * \param[in] mesh    Source Assimp mesh data
     * \param[in] scene   Pointer to Assimp scene data
     * \return Processed Mesh object with vertices and textures
     */
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    /// Load textures from material
    /**
     * \param[in] mat       Assimp material structure
     * \param[in] type      Type of texture to load (e.g. aiTextureType_DIFFUSE)
     * \param[in] typeName  Texture type name for shader (e.g. "diffuse")
     * \return Vector of loaded Texture objects
     */
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H