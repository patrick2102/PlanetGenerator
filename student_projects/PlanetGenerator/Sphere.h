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

    /*
    glm::vec3 GetPointOnSphere(float a1, float a2)
    {
        float x = radius * glm::sin(a1) * glm::cos(a2);
        float y = radius * glm::cos(a1);
        float z = radius * glm::sin(a1) * glm::sin(a2);

        return glm::vec3(x, y, z);
    }*/

    glm::vec3 GetPointOnSphere(float a1, float a2)
    {
        float x = radius * glm::cos(a1) * glm::cos(a2);
        float y = radius * glm::sin(a1);
        float z = radius * glm::cos(a1) * glm::sin(a2);

        return glm::vec3(x, y, z);
    }

    void CreateSphere(int divisions)
    {
        float increment = pi * (1.0f/(float)divisions);

        float a1 = pi/2;

        for(int j = 0; j < divisions; j++)
        {
            for(int i = 0; i < divisions*2; i++)
            {
                float a2 = pi * ((float)i)/(float)divisions;

                glm::vec3 p1 = GetPointOnSphere(a1, a2);
                glm::vec3 p2 = GetPointOnSphere(a1-increment, a2);
                glm::vec3 p3 = GetPointOnSphere(a1-increment, a2+increment);

                vertices.insert(vertices.end(), {p2, p1, p3});

                if(j != 0 && j != divisions-1)
                {
                    glm::vec3 p4 = GetPointOnSphere(a1, a2+increment);
                    vertices.insert(vertices.end(), {p3, p1, p4});
                }
            }
            a1 -= increment;
        }

        std::cout << "triangles: " << vertices.size()/3;
    }
};
#endif //ITU_GRAPHICS_PROGRAMMING_SPHERE_H