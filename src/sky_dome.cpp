//
// Created by luky9 on 26.04.2025.
//

#include "sky_dome.h"


SkyDome::SkyDome(std::string texturePath, int shaderIdx){
    this->shaderIdx = shaderIdx;
    texture.path = texturePath;
    texture.type = "diffuse";
    texture.id = load_texture(texturePath.c_str());
    generateSphere();
}

void SkyDome::generateSphere() {
    float pi = 3.14159265359f;
     std::vector<float> vertices;
     std::vector<unsigned int> indices;

     unsigned int X_SEGMENTS = 64;
     unsigned int Y_SEGMENTS = 64;

     for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
         for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
             float xSegment = (float)x / (float)X_SEGMENTS;
             float ySegment = (float)y / (float)Y_SEGMENTS;
             float xPos = std::cos(xSegment * 2.0f * pi) * std::sin(ySegment * pi);
             float yPos = std::cos(ySegment * pi);
             float zPos = std::sin(xSegment * 2.0f * pi) * std::sin(ySegment * pi);

             // positions (x, y, z), texture coords (u, v)
             vertices.push_back(xPos);
             vertices.push_back(yPos);
             vertices.push_back(zPos);
             vertices.push_back(xSegment);
             vertices.push_back(ySegment);
         }
     }

     // Now create indices
     for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
         for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
             unsigned int i0 = y * (X_SEGMENTS + 1) + x;
             unsigned int i1 = (y + 1) * (X_SEGMENTS + 1) + x;

             indices.push_back(i0);
             indices.push_back(i1);
             indices.push_back(i0 + 1);

             indices.push_back(i0 + 1);
             indices.push_back(i1);
             indices.push_back(i1 + 1);
         }
     }

     size = indices.size();

     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);

     glBindVertexArray(VAO);

     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


     // position
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);
     // texcoords
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
     glEnableVertexAttribArray(1);
     glBindVertexArray(0);

}

void SkyDome::draw(GLuint shader, Camera* currentCamera, glm::mat4 projection) {
    glDepthMask(GL_FALSE);
    glUseProgram(shader);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, currentCamera->Position);
    model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f));

    GLint skyDome_projLoc = glGetUniformLocation( shader, "projection");
    GLint skyDome_viewLoc = glGetUniformLocation( shader, "view");
    GLint skyDome_modelLoc = glGetUniformLocation( shader, "model");

    // Uniforms
    glUniformMatrix4fv(skyDome_projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(skyDome_viewLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->getViewMatrix()));
    glUniformMatrix4fv(skyDome_modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glUniform1i(glGetUniformLocation(shader, "skyTexture"), 0);

    // Render
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}


