//
// Created by Patrick on 28/04/2022.
//

#include "Sphere.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SUN_H
#define ITU_GRAPHICS_PROGRAMMING_SUN_H

struct SunVertex
{
    glm::vec3 Position;
    glm::vec3 Color;
};

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
    std::vector<SunVertex> vertices;

    void SetUpVertices()
    {
        for(auto vPos : sphere.vertices)
        {
            //vPos += center;
            vertices.insert(vertices.end(), {vPos, baseColor});
        }
        //Clear sphere vertices to not use up wasteful memory.
        //sphere.vertices.clear();
    }

    void SetUpBuffers()
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SunVertex), &vertices[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        int posAttributeLocation = glGetAttribLocation(shader->ID, "position");
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (void *) 0);

        posAttributeLocation = glGetAttribLocation(shader->ID, "color");
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (void *) sizeof(glm::vec3));

        glBindVertexArray(0);
    }
};
#endif //ITU_GRAPHICS_PROGRAMMING_SUN_H