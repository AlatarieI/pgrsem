//
// Created by luky9 on 26.04.2025.
//

#ifndef SKY_DOME_H
#define SKY_DOME_H

#include <gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "utility.h"


class SkyBox {
public:
    Mesh* mesh;
    int shaderIdx;
    std::vector<Texture> textures;
    bool shouldBlend;
    GLuint VAO, VBO, EBO;
    int size;
    int currTextureIdx = 0;

    SkyBox(std::string texturePath, int shaderIdx);
    SkyBox(const std::vector<std::string>& texturePaths, int shaderIdx);

    void draw(GLuint shader, Camera* currentCamera, glm::mat4 projection, float blend);

private:
    void setupSkyBox();
};



#endif //SKY_DOME_H
