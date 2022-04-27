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
    std::vector<Mesh> meshes;
    Shader* shader;

    Sphere(glm::vec3 position, float radius, int divisions, Shader* shader)
    {
        this-> position = position;
        this-> radius = radius;
        this-> shader = shader;

        CreateSphere(divisions);
    }

    glm::vec3 GetPointOnSphere(float angleY, float angleZ)
    {
        float x = radius * glm::sin(angleY) * glm::cos(angleZ);
        float y = radius * glm::sin(angleY) * glm::sin(angleZ);
        float z = radius * glm::cos(angleY);

        return glm::vec3(x, y, z);
    }

    void CreateSphere(int divisions)
    {
        float increment = pi * (1.0f/(float)divisions);

        //float angleZ = pi/2;
        float angleZ = pi;

        for(int j = 0; j < (divisions*2); j++)
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

                Mesh topLeft = Mesh(std::vector<Vertex>({{p1, color}, {p2, color}, {p3, color}}),
                                    glm::cross(p1, p2), shader);

                Mesh bottomRight = Mesh(std::vector<Vertex>({{p4, color}, {p5, color}, {p6, color}}),
                                    glm::cross(p4, p5), shader);

                meshes.insert(meshes.end(), {topLeft, bottomRight});

                /*
                glm::vec3 p1 = GetPointOnSphere(angleY, angleZ);
                glm::vec3 p2 = GetPointOnSphere(angleY-increment, angleZ);
                glm::vec3 p3 = GetPointOnSphere(angleY-increment, angleZ+increment);

                Mesh temp = Mesh(std::vector<Vertex>({{p1, color}, {p2, color}, {p3, color}}),
                                 glm::cross(p1, p2), shader);

                meshes.insert(meshes.end(), {temp});
                */

            }
            angleZ -= (increment);
        }
    }

    void Draw()
    {

        shader->use();
        for(auto mesh : meshes)
        {
            mesh.Draw();
        }
    }

};