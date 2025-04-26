//
// Created by luky9 on 26.04.2025.
//

#ifndef SKY_DOME_H
#define SKY_DOME_H

#include <gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "utility.h"


class SkyDome {
public:
    Mesh* mesh;
    int shaderIdx;
    Texture texture;
    GLuint VAO, VBO, EBO;
    int size;

    SkyDome(std::string texturePath, int shaderIdx);

    void draw(GLuint shader, Camera* currentCamera, glm::mat4 projection);

private:
    void generateSphere();
};



#endif //SKY_DOME_H
