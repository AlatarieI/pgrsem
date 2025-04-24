//
// Created by skorelu1 on 31.03.2025.
//
#include "game_object.h"


GameObject::GameObject(std::string modelFilePath, bool flipUV, GLuint shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale):
                        model(modelFilePath, flipUV) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    this->shader = shader;
}

GameObject::GameObject(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, bool flipUV, GLuint shader,
                        glm::vec3 position, glm::vec3 rotation, glm::vec3 scale):
                        model(vertices, indices, textures, flipUV) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    this->shader = shader;
}

void GameObject::draw(glm::mat4 parentModel) {
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(parentModel*getModelMatrix()));
    model.draw(shader);
}

void GameObject::update(float deltaTime) {

}


glm::mat4 GameObject::getModelMatrix() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

