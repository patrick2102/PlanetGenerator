//
// Created by Patrick on 28/04/2022.
//
#include <map>
#include "HeightMapGenerator.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_PLANET_H
#define ITU_GRAPHICS_PROGRAMMING_PLANET_H

enum PlanetNames
{
    planet_test1
};

class Planet
{
public:
    Planet(glm::vec3 position, PlanetData planetData, const char* planetName)
            : planetData(planetData) {
        this->center = position;
        this->planetName = planetName;
        SetUpBuffers();
        SetUpAtmosphereBuffers();
    }

    void DrawAtmosphere(Shader *shader)
    {
        unsigned int VAO = planetData.atmosphere.VAO;
        auto vertices = planetData.atmosphere.points;

        shader->setVec3("center", planetData.atmosphere.center);
        shader->setFloat("inner_radius", planetData.atmosphere.inner_radius);
        shader->setFloat("outer_radius", planetData.atmosphere.outer_radius);
        shader->setVec3("sunPosition", planetData.atmosphere.sunPosition);

        auto model = glm::mat4 (1.0f);
        auto localSpace = glm::mat4 (1.0f);
        model = glm::translate(model, center);
        localSpace = glm::translate(model, -center);
        shader->setMat4("model", model);
        shader->setMat4("localSpace", localSpace);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);
    }

    void Draw(Shader *shader)
    {
        Displacement displacement = planetData.displacement;

        for(Material material : planetData.materials)
        {
            if(material.points.size() == 0)
                continue;

            unsigned int VAO = material.VAO;
            auto vertices = material.points;

            shader->setVec3("reflectionColor", material.reflectionColor);
            shader->setFloat("diffuseReflectance", material.diffuseReflectance);
            shader->setFloat("roughness", material.roughness);
            shader->setFloat("metalness", material.metalness);


            shader->setInt("scale", displacement.scale);
            shader->setFloat("amplitude", displacement.amplitude);
            shader->setFloat("persistence", displacement.persistence);
            shader->setFloat("lacunarity", displacement.lacunarity);
            shader->setInt("diameter", displacement.diameter);
            shader->setInt("iterations", displacement.iterations);

            auto model = glm::mat4 (1.0f);
            model = glm::translate(model, center);
            shader->setMat4("model", model);
            shader->setInt("surfaceTexture", 0);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glBindVertexArray(0);
        }
    }

private:
    unsigned int surfaceTexture;
    unsigned int displacementMap;
    unsigned int frameBuffer;

    const char* planetName;

    glm::vec3 center;
    PlanetData planetData;

    void SetUpAtmosphereBuffers()
    {
        unsigned int VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, planetData.atmosphere.points.size() * sizeof(Vertex), &planetData.atmosphere.points[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &planetData.atmosphere.VAO);
        glBindVertexArray(planetData.atmosphere.VAO);

        int posAttributeLocation = 0;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        posAttributeLocation = 1;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(glm::vec3));

        posAttributeLocation = 2;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (sizeof(glm::vec3)*2));

        glBindVertexArray(0);
    }

    void SetUpBuffers()
    {
        for(int i = 0; i < planetData.materials.size(); i++)
        {
            if(planetData.materials[i].points.size() == 0)
                continue;

            unsigned int VBO;

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, planetData.materials[i].points.size() * sizeof(Vertex), &planetData.materials[i].points[0], GL_STATIC_DRAW);

            glGenVertexArrays(1, &planetData.materials[i].VAO);
            glBindVertexArray(planetData.materials[i].VAO);

            int posAttributeLocation = 0;
            glEnableVertexAttribArray(posAttributeLocation);
            glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

            posAttributeLocation = 1;
            glEnableVertexAttribArray(posAttributeLocation);
            glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(glm::vec3));

            posAttributeLocation = 2;
            glEnableVertexAttribArray(posAttributeLocation);
            glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (sizeof(glm::vec3)*2));

            glBindVertexArray(0);
        }
    }
};
#endif //ITU_GRAPHICS_PROGRAMMING_PLANET_H