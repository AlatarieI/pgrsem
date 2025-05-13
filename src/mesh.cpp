#include "mesh.h"

#include <iostream>
#include <gtc/type_ptr.hpp>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
}

void Mesh::draw(GLuint program) {
    unsigned int diffuseNum  = 1;
    unsigned int specularNum = 1;
    for(GLint i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE1 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "diffuse")
            number = std::to_string(diffuseNum++);
        else if (name == "specular")
            number = std::to_string(specularNum++);

        glUniform1i(glGetUniformLocation(program, ("materialTexture" + number + "." + name).c_str()), i+1);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glUniform1i(glGetUniformLocation(program, "useDiffuseTexture"), useDiffuseTexture);
    glUniform1i(glGetUniformLocation(program, "useSpecularTexture"), useSpecularTexture);

    glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);


    for (int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

