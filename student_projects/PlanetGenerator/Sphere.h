//
// Created by Patrick on 27/04/2022.
//
#include "shader.h"
#include "Mesh.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_SPHERE_H

#endif //ITU_GRAPHICS_PROGRAMMING_SPHERE_H

class Sphere
{
public:
    float pi = glm::pi<float>();

    glm::vec3 position;
    glm::vec4 color = glm::vec4 (1.0f, 0.0f, 0.0f, 0.0f);

    float radius;
    std::vector<Vertex> vboVec;
    Shader* shader;

    Sphere(glm::vec3 position, float radius, int divisions, glm::vec4 baseColor, Shader* shader)
    {
        this-> position = position;
        this-> radius = radius;
        this-> shader = shader;
        this-> color = baseColor;

        CreateSphere(divisions);
        SetupBuffers();
    }

    glm::vec3 GetPointOnSphere(float angleY, float angleZ)
    {
        float x = radius * glm::sin(angleY) * glm::cos(angleZ);
        float y = radius * glm::sin(angleY) * glm::sin(angleZ);
        float z = radius * glm::cos(angleY);

        return glm::vec3(x, y, z) + position;
    }

    void CreateSphere(int divisions)
    {
        float increment = pi * (1.0f/(float)divisions);

        float angleZ = pi/2;

        for(int j = 0; j < divisions*2; j++)
        {
            for(int i = 0; i < divisions; i++)
            {
                float angleY = 2 * pi * ((float)i)/(float)divisions;

                glm::vec3 p1 = GetPointOnSphere(angleY, angleZ);
                glm::vec3 p2 = GetPointOnSphere(angleY-increment, angleZ);
                glm::vec3 p3 = GetPointOnSphere(angleY-increment, angleZ+increment);

                glm::vec3 p4 = GetPointOnSphere(angleY, angleZ);
                glm::vec3 p5 = GetPointOnSphere(angleY-increment, angleZ+increment);
                glm::vec3 p6 = GetPointOnSphere(angleY, angleZ+increment);

                vboVec.insert(vboVec.end(), {{p1, color}, {p2, color}, {p3, color}, {p4, color}, {p5, color}, {p6, color}});
            }
            angleZ -= increment;
        }
    }

    void SetupBuffers()
    {
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

    void Draw()
    {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vboVec.size());
        glBindVertexArray(0);
    }


private:
    unsigned int VAO;
    unsigned int VBO;
};