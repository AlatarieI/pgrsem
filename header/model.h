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

class Model {
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
    bool flipUV;

    Model(std::string const &path, bool flipUV = false, bool gamma = false);

    Model(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, bool flipUV = false, bool gamma = false);

    void draw(GLuint program);

    
private:
    void loadModel(std::string const &path, bool flipUV);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};


#endif