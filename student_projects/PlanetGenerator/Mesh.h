//
// Created by Patrick on 27/04/2022.
//
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_MESH_H
#define ITU_GRAPHICS_PROGRAMMING_MESH_H

#endif //ITU_GRAPHICS_PROGRAMMING_MESH_H

struct Vertex
{
    // Position
    glm::vec3 Position;

    // Color
    glm::vec4 Color;
};


class Mesh
{
public:
    unsigned int VAO;
    unsigned int VBO;
    glm::vec3 normal;
    std::vector<Vertex> vboVec;


    Mesh(std::vector<Vertex> vboVec, glm::vec3 normal, Shader* shader)
    {
        this->normal = normal;
        this->vboVec = std::move(vboVec);
        Setup(shader);
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vboVec.size());
    }

private:
    void Setup(Shader* shader) {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vboVec.size() * sizeof(Vertex), &vboVec[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        int offset = 0;
        //int posAttributeLocation = glGetAttribLocation(shader->ID, "pos");
        int posAttributeLocation = 0;

        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offset);

        GLenum code = glGetError();
        if (code != GL_NO_ERROR)
            std::cerr << "glVertexAttribPointer() with a stride of " << sizeof(Vertex) << " failed with code " << code
                      << std::endl;

        offset += sizeof(glm::vec3);
        posAttributeLocation = glGetAttribLocation(shader->ID, "color");

        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offset);

        glBindVertexArray(0);
    }
};