//
// Created by Patrick on 28/04/2022.
//
#include <map>
#include "Sphere.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_PLANET_H
#define ITU_GRAPHICS_PROGRAMMING_PLANET_H

enum PlanetNames
{
    planet_test1
};

class Planet
{
public:
    Planet(glm::vec3 position, CubeSphere sphere, PlanetData planetData, const char* planetName)
            : planetData(planetData) {
        this->center = position;
        this->planetName = planetName;
        //SetUpVertices(sphere);
        //SetUpOceanVertices(sphere);
        SetUpBuffers();
        SetUpOceanBuffers();
        //SetUpTextures();
    }

    void DrawOceanUsingGPU(Shader *shader)
    {
        Displacement displacement = planetData.displacement;

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
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glBindVertexArray(0);
        }
    }

    void Draw(Shader *shader)
    {
        Displacement displacement = planetData.displacement;

        for(Material material : planetData.materials) {

            unsigned int VAO = material.VAO;
            auto vertices = material.points;

            shader->setVec3("reflectionColor", material.reflectionColor);
            shader->setFloat("diffuseReflectance", material.diffuseReflectance);
            shader->setFloat("roughness", material.roughness);
            shader->setFloat("metalness", material.metalness);

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, center);
            shader->setMat4("model", model);
            shader->setInt("surfaceTexture", 0);
            shader->setInt("displacementMap", 0);

            glBindVertexArray(VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, surfaceTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, displacementMap);
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

    const char* planetName;
    bool loadHeightMap = true;
    bool useGPU = false;
    const char* folder = "planetNoise/";

    glm::vec3 center;
    PlanetData planetData;
    std::vector<Vertex> oceanVertices;

    void SetUpOceanVertices(CubeSphere sphere)
    {
        for(int i = 0; i < sphere.vertices.size(); i += 6)
        {
            auto p1 = sphere.vertices[i];
            auto p2 = sphere.vertices[i+1];
            auto p3 = sphere.vertices[i+2];

            auto p4 = sphere.vertices[i+3];
            auto p5 = sphere.vertices[i+4];
            auto p6 = sphere.vertices[i+5];

            oceanVertices.insert(oceanVertices.end(), {p1, p1});
            oceanVertices.insert(oceanVertices.end(), {p2, p2});
            oceanVertices.insert(oceanVertices.end(), {p3, p3});
            oceanVertices.insert(oceanVertices.end(), {p4, p4});
            oceanVertices.insert(oceanVertices.end(), {p5, p5});
            oceanVertices.insert(oceanVertices.end(), {p6, p6});
        }
    }

    void SetUpOceanBuffers()
    {
        unsigned int VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, planetData.ocean.material.points.size() * sizeof(Vertex), &planetData.ocean.material.points[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &planetData.ocean.material.VAO);
        glBindVertexArray(planetData.ocean.material.VAO);

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

    void SetupFrameBuffer(int d)
    {
        //GLuint fbo_handle;
        GLuint FBO;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, d, d, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        displacementMap = textureID;
    }

/*
    void SetUpTextures(Shader *shader) {
        std::vector<std::string> faces;
        float seed = 2.0;

        if (!loadHeightMap) {

            //Initial values for height map
            HeightMapGenerator hmg = HeightMapGenerator(seed);

            //For generating heightmap
            int scale = 250;
            float amplitude = 10.0f;
            float persistence = 0.5;
            float lacunarity = 2.0;
            int w = 1000;
            int h = w;
            int d = w;
            int iterations = 6;
            float r = 10;

            float **heightMap = hmg.GenerateMap(w, h, iterations, scale, amplitude, persistence, lacunarity);
            std::vector<float **> heightCubeMap = hmg.GenerateCubeMap(d, iterations, scale, amplitude,
                                                                       persistence, lacunarity);

            auto outputByteFile = hmg.OutputImage(d, heightMap, planetName);
            //auto outputFloatFile = hmg.OutputImageFloat(w, h, heightMap, planetName);
            auto outputFilesByte = hmg.OutputCubeMapImage(d, heightCubeMap, planetName);
            faces = outputFilesByte;
        } else {
            HeightMapGenerator hmg = HeightMapGenerator(seed);
            faces =
                    {
                            "planetNoise/b_cube/PosX_test_1_float.bmp",
                            "planetNoise/b_cube/NegX_test_1_float.bmp",
                            "planetNoise/b_cube/PosY_test_1_float.bmp",
                            "planetNoise/b_cube/NegY_test_1_float.bmp",
                            "planetNoise/b_cube/PosZ_test_1_float.bmp",
                            "planetNoise/b_cube/NegZ_test_1_float.bmp"
                    };
        }


        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        //unsigned char* data = stbi_load(outputByteFile.c_str(), &width, &height, &nrComponents, 0);
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            int width, height, nrComponents;
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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
        displacementMap = textureID;
    }
*/
};
#endif //ITU_GRAPHICS_PROGRAMMING_PLANET_H