//
// Created by luky9 on 24.04.2025.
//

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H
#include <glm.hpp>
#include <string>


class SceneObject {
public:

    GLuint shader;

    SceneObject parent;

    std::string name;
    glm::vec3 position;
    int modelIndex;  // Index into models

};



#endif //SCENE_OBJECT_H
