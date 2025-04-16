//
// Created by skorelu1 on 31.03.2025.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <optional>
#include <string>

#include <gtc/type_ptr.hpp>

#include "model.h"

class GameObject {
public:
    GLuint shader;
    Model model;
    glm::vec3 position{}, rotation{}, scale{};

    GameObject(std::string modelFilePath, bool flipUV, GLuint shader, glm::vec3 position,
                glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

    GameObject(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, bool flipUV, GLuint shader,
                glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

    void draw(glm::mat4 parentModel = glm::mat4(1.0f));

    void update(float deltaTime);

    glm::mat4 getModelMatrix();

};



#endif //GAMEOBJECT_H
