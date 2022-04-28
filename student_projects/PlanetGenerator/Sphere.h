//
// Created by Patrick on 27/04/2022.
//
#include "shader.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_SPHERE_H


class Sphere
{
public:
    float pi = glm::pi<float>();
    float radius;
    std::vector<glm::vec3> vertices;

    Sphere(float radius, int divisions)
    {
        this-> radius = radius;

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

                glm::vec3 normal1 = glm::cross(p1-p2, p1-p3);
                glm::vec3 normal2 = glm::cross(p4-p5, p4-p6);

                vertices.insert(vertices.end(), {p1, p2, p3, p4, p5, p6});
            }
            angleZ -= increment;
        }
    }
};
#endif //ITU_GRAPHICS_PROGRAMMING_SPHERE_H