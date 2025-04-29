//
// Created by luky9 on 24.04.2025.
//

#include "lights.h"

#include <iostream>


void DirectionalLight::setUniforms(GLuint lightingShader) {
    glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.direction"), 1, glm::value_ptr(direction));
    glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.ambient"), 1, glm::value_ptr(ambient));
    glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.diffuse"), 1, glm::value_ptr(diffuse));
    glUniform3fv(glGetUniformLocation(lightingShader, "dirLight.specular"), 1, glm::value_ptr(specular));
}


void PointLight::setUniforms(GLuint lightingShader, int index) {
    std::string idx = "pointLights[" + std::to_string(index) + "]";
    glUniform3fv(glGetUniformLocation(lightingShader, (idx + ".position").c_str()), 1, glm::value_ptr(position));
    glUniform3fv(glGetUniformLocation(lightingShader, (idx + ".ambient").c_str()), 1, glm::value_ptr(ambient));
    glUniform3fv(glGetUniformLocation(lightingShader, (idx + ".diffuse").c_str()), 1, glm::value_ptr(diffuse));
    glUniform3fv(glGetUniformLocation(lightingShader, (idx + ".specular").c_str()), 1, glm::value_ptr(specular));
    glUniform1f(glGetUniformLocation(lightingShader, (idx + ".constant").c_str()), constant);
    glUniform1f(glGetUniformLocation(lightingShader, (idx + ".linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(lightingShader, (idx + ".quadratic").c_str()), quadratic);

}


void SpotLight::setUniforms(GLuint lightingShader) {
    glUniform3fv(glGetUniformLocation(lightingShader, "spotLight.position"), 1, glm::value_ptr(position));
    glUniform3fv(glGetUniformLocation(lightingShader, "spotLight.direction"), 1, glm::value_ptr(direction));
    glUniform3fv(glGetUniformLocation(lightingShader, "spotLight.ambient"), 1, glm::value_ptr(ambient));
    glUniform3fv(glGetUniformLocation(lightingShader, "spotLight.diffuse"), 1, glm::value_ptr(diffuse));
    glUniform3fv(glGetUniformLocation(lightingShader, "spotLight.specular"), 1., glm::value_ptr(specular));
    glUniform1f(glGetUniformLocation(lightingShader, "spotLight.constant"), constant);
    glUniform1f(glGetUniformLocation(lightingShader, "spotLight.linear"), linear);
    glUniform1f(glGetUniformLocation(lightingShader, "spotLight.quadratic"), quadratic);
    glUniform1f(glGetUniformLocation(lightingShader, "spotLight.cutOff"), glm::cos(glm::radians(cutOff)));
    glUniform1f(glGetUniformLocation(lightingShader, "spotLight.outerCutOff"), glm::cos(glm::radians(outerCutOff)));

}

