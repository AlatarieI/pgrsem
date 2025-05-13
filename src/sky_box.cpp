#include "sky_box.h"

SkyBox::SkyBox(const std::vector<std::string>& texturePaths, int shaderIdx) {
    this->shaderIdx = shaderIdx;
    for (const auto& path: texturePaths) {
        Texture texture;
        texture.path = path;
        texture.type = "skybox";
        texture.id = load_cube_map(path.c_str());
        textures.push_back(texture);
    }
    if (textures.size() > 1) {
        shouldBlend = true;
    }
    setupSkyBox();
}




void SkyBox::setupSkyBox() {
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void SkyBox::draw(GLuint shader, Camera& currentCamera, glm::mat4 projection, float blend) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shader);

    glm::mat4 view = glm::mat4(glm::mat3(currentCamera.getViewMatrix())); // remove translation from the view matrix

    GLint skyDome_projLoc = glGetUniformLocation( shader, "projection");
    GLint skyDome_viewLoc = glGetUniformLocation( shader, "view");

    GLint skyDome_blendLoc = glGetUniformLocation( shader, "blend");

    // Uniforms
    glUniformMatrix4fv(skyDome_projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(skyDome_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1f(skyDome_blendLoc, blend);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[currTextureIdx].id);
    glUniform1i(glGetUniformLocation(shader, "skyTexture1"), 0);

    glActiveTexture(GL_TEXTURE1);
    if (shouldBlend)
        glBindTexture(GL_TEXTURE_CUBE_MAP, textures[(currTextureIdx + 1)%textures.size()].id);
    else
        glBindTexture(GL_TEXTURE_CUBE_MAP, textures[currTextureIdx].id);
    glUniform1i(glGetUniformLocation(shader, "skyTexture2"), 1);


    // skybox cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}


