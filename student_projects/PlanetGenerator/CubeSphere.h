//
// Created by Patrick on 27/04/2022.
//

#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

class CubeSphere
{
public:
    float pi = glm::pi<float>();
    glm::vec3 position;
    glm::vec3 normal;
    std::vector<Mesh> sides;
    Shader* shader;
    float radius;

    CubeSphere(glm::vec3 position, float radius, int divisions, Shader* shader)
    {
        this->position = position;
        this->radius = radius;
        this->shader = shader;
        normal = glm::vec3(0.0f, 0.0f, -1.0f);
        CreateSphereCube(radius, divisions);
    }

    void Draw()
    {
        glm::mat4 model = glm::mat4(1.0f);

        shader->use();

        for(auto side : sides)
        {
            side.Draw();
        }
    }

private:
    int divisions;

    void CreateCube(float width)
    {
        sides.clear();

        float rotation = 0.0f;
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        for(int i = 0; i < 4; i++)
        {
            Mesh side = SideOfSquare(width, rotation, rotationAxis);
            sides.insert(sides.end(), side);
            rotation += pi/2;
        }

        rotation = pi/2;
        rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);

        for(int i = 0; i < 2; i++)
        {
            Mesh side = SideOfSquare(width, rotation, rotationAxis);
            sides.insert(sides.end(), side);
            rotation += pi;
        }
    }

    Mesh SideOfSquare(float width, float rotation, glm::vec3 rotationAxis) {
        // The 4 different corners of the square
        float p0x = -width / 2, p0y =
                -width / 2, p0z = 0; // p0x, p0y are the lowest x and y values, p0z is 0 before transformation.
        float p1x = width / 2, p1y = width / 2; // p1x and p1y are the highest x and y values.

        glm::mat4 model = glm::mat4(1.0f);
        auto rotateTransform = glm::rotate(model, rotation, rotationAxis);

        glm::vec3 sideNormal = rotateTransform * glm::vec4(normal, 0.0f);

        auto translateTransform = glm::translate(model, sideNormal * (width / 2));

        model = glm::rotate(translateTransform, rotation, rotationAxis);

        glm::vec3 topLeft = model * glm::vec4(p0x, p1y, p0z, 1.0f);
        glm::vec3 topRight = model * glm::vec4(p1x, p1y, p0z, 1.0f);
        glm::vec3 bottomLeft = model * glm::vec4(p0x, p0y, p0z, 1.0f);
        glm::vec3 bottomRight = model * glm::vec4(p1x, p0y, p0z, 1.0f);

        //std::vector<glm::vec3> coords;
        std::vector<Vertex> vertices;

        glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        // Top left triangle
        vertices.insert(vertices.end(), {topLeft, red});
        vertices.insert(vertices.end(), {topRight, red});
        vertices.insert(vertices.end(), {bottomLeft, red});

        //Bottom right triangle
        vertices.insert(vertices.end(), {bottomRight, red});
        vertices.insert(vertices.end(), {bottomLeft, red});
        vertices.insert(vertices.end(), {topRight, red});

        Mesh side = Mesh(vertices, sideNormal, shader);
        return side;
    }

    void CreateSphereFromCube(int cubeDivisions)
    {

        float lonAngle;
        float latAngle;

        int intersections = (int)pow(2, cubeDivisions);

        for (int i = 0; i <= intersections; i++)
        {
            lonAngle = (pi/4 - pi/2 * (float)i)/((float)intersections);
            glm::vec3 lonNormal = glm::vec3 (-glm::sin(lonAngle), glm::cos(lonAngle), 0);
            for (int j = 0; j < intersections; j++)
            {
                latAngle = (pi/4 - pi/2 * (float)j)/((float)intersections);
                glm::vec3 latNormal = glm::vec3 (-glm::sin(latAngle), 0, glm::cos(latAngle));

                glm::vec3 v_dir = glm::normalize(glm::cross(lonNormal, latNormal));
            }
        }
    }

    void CreateSphereCube(float radius, int divisions)
    {
        CreateCube(radius);
        this->divisions = divisions;
    }
};