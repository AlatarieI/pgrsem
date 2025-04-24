//
// Created by luky9 on 24.04.2025.
//

#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm.hpp>

class DirectionalLight{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 direction;

    void setUniforms(GLuint lightingShader);
};

class PointLight{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    void setUniforms(GLuint lightingShader);
};

class SpotLight{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    void setUniforms(GLuint lightingShader);
};




#endif //LIGHT_H
