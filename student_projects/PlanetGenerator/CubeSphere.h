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

    // Color
    //glm::vec4 Color;
};

class Side
{
public:
    unsigned int VAO;
    unsigned int VBO;
    glm::vec3 normal;
    std::vector<Vertex> vboVec;


    Side(std::vector<Vertex> vboVec, glm::vec3 normal, Shader* shader)
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
    void Setup(Shader* shader)
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
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offset);

        GLenum code = glGetError();
        if (code != GL_NO_ERROR)
            std::cerr << "glVertexAttribPointer() with a stride of " << sizeof(Vertex) << " failed with code " << code << std::endl;
        /*
        offset += sizeof(glm::vec3);
        posAttributeLocation = glGetAttribLocation(shader->ID, "color");

        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offset);
        */
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

class CubeSphere
{
public:
    float pi = glm::pi<float>();
    glm::vec3 position;
    glm::vec3 normal;
    std::vector<Side> sides;
    Shader* shader;
    float radius;

    CubeSphere(glm::vec3 position, float radius, Shader* shader)
    {
        this->position = position;
        this->radius = radius;
        this->shader = shader;
        normal = glm::vec3(0.0f, 0.0f, -1.0f);
        CreateCube(radius);
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

        sideNormal = round(sideNormal);

        model = glm::translate(model, sideNormal);

        glm::vec3 topLeft = model * glm::vec4 (p0x, p1y, p0z, 1.0f);
        glm::vec3 topRight = model * glm::vec4 (p1x, p1y, p0z, 1.0f);
        glm::vec3 bottomLeft = model * glm::vec4 (p0x, p0y, p0z, 1.0f);
        glm::vec3 bottomRight = model * glm::vec4 (p1x, p0y, p0z, 1.0f);

        //std::vector<glm::vec3> coords;
        std::vector<Vertex> vertices;

        glm::vec4 red = glm::vec4 (1.0f, 0.0f, 0.0f, 1.0f);

        // Top left triangle
        vertices.insert(vertices.end(), {topLeft});
        vertices.insert(vertices.end(), {topRight});
        vertices.insert(vertices.end(), {bottomLeft});

        //Bottom right triangle
        vertices.insert(vertices.end(), {bottomRight});
        vertices.insert(vertices.end(), {bottomLeft});
        vertices.insert(vertices.end(), {topRight});

        Side side = Side(vertices, sideNormal, shader);
        return side;
    }
};