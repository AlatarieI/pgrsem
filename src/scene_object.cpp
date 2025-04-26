//
// Created by luky9 on 24.04.2025.
//

#include "scene_object.h"

SceneObject::SceneObject() {
    shaderIndex = 0;
    parent = nullptr;
}

SceneObject::SceneObject(int shaderIndex,  int modelIndex, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale) {
    this->shaderIndex = shaderIndex;
    this->modelIndex = modelIndex;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    parent = nullptr;
}

glm::mat4 SceneObject::getModelMatrix() const {
    auto modelMatrix = parent != nullptr ? parent->getModelMatrix() : glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}

