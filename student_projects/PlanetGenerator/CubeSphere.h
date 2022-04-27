//
// Created by Patrick on 27/04/2022.
//

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

class Side
{
public:
    unsigned int VBO;
    std::vector<glm::vec3> vboVec;

    Side(std::vector<glm::vec3> vboVec)
    {
        vboVec = vboVec;
        Setup();
    }

    void Setup()
    {
        //glGenBuffers(1, &VBO);
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //glBufferData(GL_ARRAY_BUFFER, vboVec.size() * sizeof(Vertex), &vboVec[0], GL_STATIC_DRAW);

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
    float radius;
    std::vector<Side> sides;

    CubeSphere(glm::vec3 position, float radius)
    {
        position = position;
        radius = radius;
        //sides = CreateCubeSphere(0);
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

    std::vector<glm::vec3> CreateCube(float width)
    {
        //TODO transformation in shader
        std::vector<float> vboVec;

        std::vector<glm::vec3> rightFace = FaceOfSquare(width);
        /*
        std::vector<glm::vec3> leftFace = FaceOfSquare(width);
        std::vector<glm::vec3> topFace = FaceOfSquare(width);
        std::vector<glm::vec3> bottomFace = FaceOfSquare(width);
        std::vector<glm::vec3> frontFace = FaceOfSquare(width);
        std::vector<glm::vec3> backFace = FaceOfSquare(width);
        */

        glm::mat4 model = glm::mat4(1.0f);

        float radiansToRotate = pi/2;
        glm::vec3 front = glm::vec3(0.0f, 1.0f, 0.0f);

        auto rotation = glm::rotate(model, radiansToRotate, glm::vec3(0.0f, 0.0f, 0.0f));

        /*
        for(int i = 0; i < 6; i++)
        {
            auto res = rotation * rightFace[i];
        }
        */
        return rightFace;
    }

    std::vector<glm::vec3> FaceOfSquare(float width)
    {

        std::vector<glm::vec3> coords;

        // The 4 different corners of the square
        float p0x = -width/2, p0y = -width/2, p0z = width/2; // p0x, p0y are the lowest x and y values, p0z is 0 before transformation.
        float p1x = width/2, p1y = width/2; // p1x and p1y are the highest x and y values.


        // Top left triangle
        coords.insert(coords.end(), glm::vec3(p0x, p1y, p0z)); // Top left corner
        coords.insert(coords.end(), glm::vec3(p1x, p1y, p0z)); // Top right corner
        coords.insert(coords.end(), glm::vec3(p0x, p0y, p0z)); // Bottom left corner


        //Bottom right triangle
        coords.insert(coords.end(), glm::vec3(p1x, p0y, p0z)); // Bottom right corner
        coords.insert(coords.end(), glm::vec3(p0x, p0y, p0z)); // Bottom left corner
        coords.insert(coords.end(), glm::vec3(p1x, p1y, p0z)); // Top right corner

        //Side side = Side()

        return coords;
    }

    std::vector<glm::vec3> CreateCubeSphere(int divisionLevels)
    {
        //glGenVertexArrays(1, &VBO);
        std::vector<glm::vec3> cube = CreateCube(radius);
        return cube;
    }
};