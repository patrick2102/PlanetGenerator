//
// Created by Patrick on 28/04/2022.
//
#include <map>
#include "Sphere.h"
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
    Planet(glm::vec3 position, PlanetData planetData, const char* planetName, HeightMapGenerator hmg)
            : planetData(planetData), hmg(hmg) {
        this->center = position;
        this->planetName = planetName;
        SetUpBuffers();
        SetUpAtmosphereBuffers();
        this->hmg = hmg;
    }

    void DrawAtmosphereUsingGPU(Shader *shader)
    {
        //hmg.CopyToShader(shader);
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

    void DrawOceanUsingGPU(Shader *shader)
    {
        Displacement displacement = planetData.displacement;
        //hmg.CopyToShader(shader);

        unsigned int VAO = planetData.ocean.material.VAO;
        auto vertices = planetData.ocean.material.points;
        Material material = planetData.ocean.material;

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

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);
    }

    void DrawUsingGPU(Shader *shader)
    {
        Displacement displacement = planetData.displacement;
        //hmg.CopyToShader(shader);

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
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, surfaceTexture);
            //glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, displacementMap);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glBindVertexArray(0);
        }
    }

    void SetUpDisplacementMap(std::vector<std::string> displacementFaces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        //unsigned char* data = stbi_load(outputByteFile.c_str(), &width, &height, &nrComponents, 0);
        for (unsigned int i = 0; i < displacementFaces.size(); i++)
        {
            int width, height, nrComponents;
            unsigned char* data = stbi_load(displacementFaces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << displacementFaces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        displacementMap = textureID;
        surfaceTexture = textureID;
    }

    void SetUpSurfaceTexture(std::vector<std::string> surfaceFaces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        //unsigned char* data = stbi_load(outputByteFile.c_str(), &width, &height, &nrComponents, 0);
        for (unsigned int i = 0; i < surfaceFaces.size(); i++)
        {
            int width, height, nrComponents;
            unsigned char* data = stbi_load(surfaceFaces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << surfaceFaces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        surfaceTexture = textureID;
    }

    void LoadTextures()
    {
        std::vector<std::string> fileNames;
        std::string outputFileName= folder;
        outputFileName.append("cube/PosX_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        outputFileName= folder;
        outputFileName.append("cube/NegX_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        outputFileName= folder;
        outputFileName.append("cube/PosY_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        outputFileName= folder;
        outputFileName.append("cube/NegY_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        outputFileName= folder;
        outputFileName.append("cube/PosZ_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        outputFileName= folder;
        outputFileName.append("cube/NegZ_");
        outputFileName.append(planetName);
        fileNames.push_back(outputFileName);

        SetUpDisplacementMap(fileNames);
    }

private:
    unsigned int surfaceTexture;
    unsigned int displacementMap;
    unsigned int frameBuffer;
    HeightMapGenerator hmg;

    const char* planetName;
    bool loadHeightMap = true;
    bool useGPU = false;
    const char* folder = "planetNoise/";

    glm::vec3 center;
    PlanetData planetData;
    std::vector<Vertex> oceanVertices;

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