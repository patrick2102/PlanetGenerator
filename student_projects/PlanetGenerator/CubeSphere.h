//
// Created by Patrick on 27/04/2022.
//

#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

struct Vertex
{
    // Position
    glm::vec3 Position;
};

class Side
{
public:
    unsigned int VBO;
    glm::vec3 normal;
    std::vector<glm::vec3> vboVec;


    Side(std::vector<glm::vec3> vboVec, glm::vec3 normal)
    {
        this->normal = normal;
        this->vboVec = std::move(vboVec);
        Setup();
    }

    void Setup()
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vboVec.size() * sizeof(Vertex), &vboVec[0], GL_STATIC_DRAW);

    }

    void Draw()
    {
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }
};

class CubeSphere
{
public:
    float pi = glm::pi<float>();
    glm::vec3 position;
    glm::vec3 normal;
    std::vector<Side> sides;
    float radius;

    CubeSphere(glm::vec3 position, float radius)
    {
        this->position = position;
        this->radius = radius;
        normal = glm::vec3(0.0f, 0.0f, -0.0f);
        CreateCube(radius);
    }

    void Draw(Shader* shader)
    {
        glm::mat4 model = glm::mat4(1.0f);

        glUseProgram(shader->ID);

        for(auto side : sides)
        {
            side.Draw();
        }
    }

private:

    void CreateCube(float width)
    {
        sides.clear();

        float rotation = 0.0f;
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        for(int i = 0; i < 4; i++)
        {
            Side side = SideOfSquare(width, rotation, rotationAxis);
            sides.insert(sides.end(), side);
            rotation += pi/2;
        }

        rotation = pi/2;
        rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);

        for(int i = 0; i < 2; i++)
        {
            Side side = SideOfSquare(width, rotation, rotationAxis);
            sides.insert(sides.end(), side);
            rotation += pi;
        }
    }

    Side SideOfSquare(float width, float rotation, glm::vec3 rotationAxis)
    {
        // The 4 different corners of the square
        float p0x = -width/2, p0y = -width/2, p0z = 0; // p0x, p0y are the lowest x and y values, p0z is 0 before transformation.
        float p1x = width/2, p1y = width/2; // p1x and p1y are the highest x and y values.

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, rotation, rotationAxis);

        glm::vec3 sideNormal = model * glm::vec4(normal,0.0f);
        model = glm::translate(model, sideNormal);

        glm::vec3 topLeft = model * glm::vec4 (p0x, p1y, p0z, 1.0f);
        glm::vec3 topRight = model * glm::vec4 (p1x, p1y, p0z, 1.0f);
        glm::vec3 bottomLeft = model * glm::vec4 (p0x, p0y, p0z, 1.0f);
        glm::vec3 bottomRight = model * glm::vec4 (p1x, p0y, p0z, 1.0f);

        std::vector<glm::vec3> coords;

        // Top left triangle
        coords.insert(coords.end(), topLeft); // Top left corner
        coords.insert(coords.end(), topRight); // Top right corner
        coords.insert(coords.end(), bottomLeft); // Bottom left corner

        //Bottom right triangle
        coords.insert(coords.end(), bottomRight); // Bottom right corner
        coords.insert(coords.end(), bottomLeft); // Bottom left corner
        coords.insert(coords.end(), topRight); // Top right corner

        Side side = Side(coords, sideNormal);

        return side;
    }

};