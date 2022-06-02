//
// Created by Patrick on 28/04/2022.
//

#include "Sphere.h"
#include "CubeSphere.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SUN_H
#define ITU_GRAPHICS_PROGRAMMING_SUN_H

class Sun
{
public:
    Sun(glm::vec3 position, CubeSphere cubeSphere, StarData starData)
            : starData(starData) {
        this->center = position;
        SetUpVerticesCubeSphere(cubeSphere);
        SetUpBuffers();
    }

    void DrawUsingGPU(Shader* shader)
    {
        StarMaterial material = starData.material;

        shader->setVec3("ambientLightColor", light.lightColor * light.lightIntensity);

        shader->setVec3("reflectionColor", material.reflectionColor);
        shader->setFloat("ambientReflectance", material.ambientReflectance);
        shader->setFloat("diffuseReflectance", material.diffuseReflectance);
        shader->setFloat("specularReflectance", material.specularReflectance);
        shader->setFloat("specularExponent", material.specularExponent);
        shader->setFloat("roughness", material.roughness);
        shader->setFloat("metalness", material.metalness);


        Displacement displacement = starData.displacement;

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

        shader->setVec3("ambientLightColor", glm::vec3(0.0f));
    }

    void Draw(Shader* shader)
    {
        StarMaterial material = starData.material;

        shader->setVec3("ambientLightColor", light.lightColor * light.lightIntensity);

        shader->setVec3("reflectionColor", material.reflectionColor);
        shader->setFloat("ambientReflectance", material.ambientReflectance);
        shader->setFloat("diffuseReflectance", material.diffuseReflectance);
        shader->setFloat("specularReflectance", material.specularReflectance);
        shader->setFloat("specularExponent", material.specularExponent);

        auto model = glm::mat4 (1.0f);
        model = glm::translate(model, center);
        shader->setMat4("model", model);


        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);

        shader->setVec3("ambientLightColor", glm::vec3(0.0f));
    }

    Light getLight()
    {
        return light;
    }

    glm::vec3 GetPosition()
    {
        return center;
    }

private:
    unsigned int VAO;
    glm::vec3 center;
    std::vector<Vertex> vertices;
    StarData starData;


    void SetUpVerticesSphere(Sphere sphere)
    {
        for(int i = 0; i < sphere.vertices.size(); i += 3)
        {
            auto p1 = sphere.vertices[i];
            auto p2 = sphere.vertices[i+1];
            auto p3 = sphere.vertices[i+2];

            auto v1 = glm::normalize(p2-p1);
            auto v2 = glm::normalize(p3-p1);

            auto normal = glm::normalize(glm::cross(v1, v2));

            vertices.insert(vertices.end(), {p1, normal});
            vertices.insert(vertices.end(), {p2, normal});
            vertices.insert(vertices.end(), {p3, normal});
        }

        //Clear sphere vertices to not use up wasteful memory.
        sphere.vertices.clear();
    }

    void SetUpVerticesCubeSphere(CubeSphere sphere)
    {
        for (int i = 0; i < sphere.vertices.size(); i += 3)
        {
            auto p1 = sphere.vertices[i];
            auto p2 = sphere.vertices[i + 1];
            auto p3 = sphere.vertices[i + 2];

            auto v1 = p2 - p1;
            auto v2 = p3 - p1;

            auto normal = glm::normalize(glm::cross(v1, v2));

            vertices.insert(vertices.end(), { p1, p1 });
            vertices.insert(vertices.end(), { p2, p1 });
            vertices.insert(vertices.end(), { p3, p1 });
        }

        //Clear sphere vertices to not use up wasteful memory.
        sphere.vertices.clear();
    }

    void SetUpBuffers()
    {
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

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

    void SetUpAtmosphereBuffers()
    {
        unsigned int VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, starData.atmosphere.points.size() * sizeof(Vertex), &starData.atmosphere.points[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &starData.atmosphere.VAO);
        glBindVertexArray(starData.atmosphere.VAO);

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
};
#endif //ITU_GRAPHICS_PROGRAMMING_SUN_H