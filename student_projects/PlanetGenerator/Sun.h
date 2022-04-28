//
// Created by Patrick on 28/04/2022.
//

#include "Sphere.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SUN_H
#define ITU_GRAPHICS_PROGRAMMING_SUN_H

class Sun
{
public:

    Sun(Sphere sphere, glm::vec3 position, glm::vec3 color, Shader* shader) : sphere(sphere)
    {
        this->center = position;
        this->baseColor = color;
        this->shader = shader;
        SetUpVertices();
        SetUpBuffers();
    }

    void Draw()
    {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);
    }

private:
    unsigned int VAO;
    unsigned int VBO;

    Sphere sphere;
    glm::vec3 center;
    glm::vec3 baseColor;
    Shader* shader;
    std::vector<Vertex> vertices;

    void SetUpVertices()
    {
        for(int i = 0; i < sphere.vertices.size(); i += 3)
        {
            auto p1 = sphere.vertices[i];
            auto p2 = sphere.vertices[i+1];
            auto p3 = sphere.vertices[i+2];

            auto normal = glm::cross(p1, p2);

            vertices.insert(vertices.end(), {p1+center, normal});
            vertices.insert(vertices.end(), {p2+center, normal});
            vertices.insert(vertices.end(), {p3+center, normal});
        }

        //Clear sphere vertices to not use up wasteful memory.
        sphere.vertices.clear();
    }

    void SetUpBuffers()
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        int posAttributeLocation = glGetAttribLocation(shader->ID, "vertex");
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        posAttributeLocation = glGetAttribLocation(shader->ID, "normal");
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(glm::vec3));

        glBindVertexArray(0);
    }
};
#endif //ITU_GRAPHICS_PROGRAMMING_SUN_H