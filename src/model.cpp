#include "model.h"

Model::Model(std::string const &path, bool flipUV, bool gamma) : gammaCorrection(gamma), flipUV(flipUV), aabbMin(glm::vec3(FLT_MAX)), aabbMax(glm::vec3(FLT_MIN)) {
    loadModel(path, flipUV);
}

Model::Model(std::vector<Mesh> myMeshes, bool flipUV, bool gamma) : gammaCorrection(gamma), flipUV(flipUV), aabbMin(glm::vec3(FLT_MAX)), aabbMax(glm::vec3(FLT_MIN)) {
    meshes.insert(meshes.end(), myMeshes.begin(), myMeshes.end());
}


void Model::draw(GLuint program) {
    for(unsigned int i = 0; i < meshes.size(); i++) {
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "useGammaCorrection"), gammaCorrection);
        meshes[i].draw(program);
    }
}

void Model::loadModel(std::string const &path, bool flipUV) {
    Assimp::Importer importer;
    auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals;
    if (flipUV)
        flags = flags | aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path, flags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    printf("Loaded model %s\n", path.c_str());
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}


void Model::processNode(aiNode *node, const aiScene *scene) {
    printf("Processing node %s, meshes %d\n", node->mName.data, node->mNumMeshes);
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        aabbMin = glm::min(aabbMin, vertex.Position);
        aabbMax = glm::max(aabbMax, vertex.Position);

        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


    Mesh resultMesh(vertices, indices, textures);

    resultMesh.useDiffuseTexture = !diffuseMaps.empty();
    resultMesh.useSpecularTexture = !specularMaps.empty();

    Material meshMaterial;

    aiColor3D color(0.f, 0.f, 0.f);
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        meshMaterial.diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        meshMaterial.specular = glm::vec3(color.r, color.g, color.b);
    }

    float shininess = 0.0f;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        meshMaterial.shininess =  shininess;
    }

    resultMesh.material = meshMaterial;

    return resultMesh;
}



std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(auto & j : textures_loaded) {
            if(std::strcmp(j.path.data(), str.C_Str()) == 0) {
                textures.push_back(j);
                skip = true;
                break;
            }
        }
        if(!skip) {
            Texture texture;
            std::string filepath = this->directory + "/" + std::string(str.C_Str());
            texture.id = load_texture(filepath.c_str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}