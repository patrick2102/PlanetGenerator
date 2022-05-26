//
// Created by Patrick on 28/04/2022.
//
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
    Planet(glm::vec3 position, Shader *shader, CubeSphere sphere, Material material, const char* planetName)
            : material(material) {
        this->center = position;
        this->shader = shader;
        this->planetName = planetName;
        SetUpVertices(sphere);
        SetUpBuffers();
        SetUpTextures();
    }
    void Draw()
    {
        shader->setVec3("reflectionColor", material.reflectionColor);
        shader->setFloat("ambientReflectance", material.ambientReflectance);
        shader->setFloat("diffuseReflectance", material.diffuseReflectance);
        shader->setFloat("specularReflectance", material.specularReflectance);
        shader->setFloat("specularExponent", material.specularExponent);

        auto model = glm::mat4 (1.0f);
        model = glm::translate(model, center);
        shader->setMat4("model", model);
        shader->setInt("surface", 0);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);


        /*
        // render skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader->use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setMat4("view", view);
        skyboxShader->setInt("skybox", 0);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        */


    }

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int texture;
    const char* planetName;

    glm::vec3 center;
    Shader* shader;
    Material material;
    std::vector<Vertex> vertices;

    void SetUpVertices(CubeSphere sphere)
    {
        for(int i = 0; i < sphere.vertices.size(); i += 3)
        {
            auto p1 = sphere.vertices[i];
            auto p2 = sphere.vertices[i+1];
            auto p3 = sphere.vertices[i+2];

            auto v1 = p2-p1;
            auto v2 = p3-p1;

            auto normal = glm::normalize(glm::cross(v1, v2));

            vertices.insert(vertices.end(), {p1, normal});
            vertices.insert(vertices.end(), {p2, normal});
            vertices.insert(vertices.end(), {p3, normal});
        }

        //Clear sphere vertices to not use up wasteful memory.
        sphere.vertices.clear();
    }

    void SetUpBuffers()
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        int posAttributeLocation = glGetAttribLocation(shader->ID, "vertex");
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        posAttributeLocation = 1;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(glm::vec3));

        glBindVertexArray(0);
    }

    void SetUpTextures()
    {
        //Initial values for height map
        double seed = 2.0;

        HeightMapGenerator hmg = HeightMapGenerator(seed);

        //For generating heightmap
        int scale = 10;
        float amplitude = 200.0f;
        double persistence = 0.5;
        double lacunarity = 2.0;
        int w = 100;
        int h = w;
        int d = w;
        int iterations = 1;
        float r = 1;

        double** heightMap = hmg.GenerateMap(w, h, iterations, scale, amplitude, persistence, lacunarity);
        std::vector<double**> heightCubeMap = hmg.GenerateCubeMap(w, h, d, r, iterations, scale, amplitude, persistence, lacunarity);

        //auto outputByteFile = hmg.OutputImage(w, h, heightMap, planetName);
        //auto outputFloatFile = hmg.OutputImageFloat(w, h, heightMap, planetName);
        auto outputFilesByte = hmg.OutputCubeMapImage(w, h, d, heightCubeMap, planetName);

        //hmg.MakeVisualization(w, h, outputFloatFile.c_str());


        /*
        std::vector<std::string> faces
                {
                        "test_1.bmp",
                        "test_1.bmp",
                        "test_1.bmp",
                        "test_1.bmp",
                        "test_1.bmp",
                        "test_1.bmp"
                };
        */

        auto faces = outputFilesByte;

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

        texture = textureID;

        /*
        for(int i = 0; i < 5; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        stbi_image_free(data);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
         */
    }

};
#endif //ITU_GRAPHICS_PROGRAMMING_PLANET_H