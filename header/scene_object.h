//
// Created by luky9 on 24.04.2025.
//

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H
#include <glm.hpp>
#include <string>
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>


class SceneObject {
public:

    int shaderIdx;

    SceneObject* parent;
    int parentIdx;

    std::string name;

    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};

    int modelIndex{};

    SceneObject();

    SceneObject(int shaderIdx, int modelIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    [[nodiscard]] glm::mat4 getModelMatrix() const;
};



#endif //SCENE_OBJECT_H
