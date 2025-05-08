//----------------------------------------------------------------------------------------
/**
 * \file       mesh.h
 * \author     skorelu1
 * \date       2025/04/10
 * \brief      3D mesh handling class with material and texture support
 *
 *  Manages vertex data, textures, and material properties for 3D rendering.
 *  Handles OpenGL buffer setup and rendering operations.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include <vector>

/// Vertex structure containing all per-vertex data
struct Vertex {
    glm::vec3 Position;  ///< Vertex position in model space
    glm::vec3 Normal;     ///< Vertex normal vector
    glm::vec2 TexCoords;  ///< Texture coordinates (UVs)
};

/// Material properties for Phong shading model
struct Material {
    glm::vec3 ambient = glm::vec3(1.0f);   ///< Ambient color reflectance
    glm::vec3 diffuse = glm::vec3(1.0f);   ///< Diffuse color reflectance
    glm::vec3 specular = glm::vec3(0.5f);  ///< Specular color reflectance
    float shininess = 32.0f;               ///< Specular exponent (size of highlight)
};

/// Texture information structure
struct Texture {
    unsigned int id;    ///< OpenGL texture ID
    std::string type;   ///< Texture type (e.g., "diffuse", "specular")
    std::string path;   ///< File path to texture source
};

/// 3D mesh class managing geometry and rendering properties
class Mesh {
public:
    std::vector<Vertex> vertices;       ///< Array of mesh vertices
    std::vector<unsigned int> indices;  ///< Vertex indices for element buffer
    std::vector<Texture> textures;      ///< Associated textures
    Material material;                  ///< Material properties
    GLuint VAO;                         ///< Vertex Array Object ID

    bool useSpecularTexture;  ///< Flag to use specular texture instead of material.specular
    bool useDiffuseTexture;   ///< Flag to use diffuse texture instead of material.diffuse

    /// Mesh constructor
    /**
     * \param[in] vertices  Array of Vertex structures
     * \param[in] indices   Element buffer indices
     * \param[in] textures  Array of Texture structures
     */
    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::vector<Texture> textures);

    /// Render the mesh
    /**
     * \param[in] program  OpenGL shader program ID to use for rendering
     */
    void draw(GLuint program);

private:
    GLuint VBO;  ///< Vertex Buffer Object ID
    GLuint EBO;  ///< Element Buffer Object ID

    /// Initialize OpenGL buffers and vertex attributes
    void setupMesh();
};

#endif // MESH_H