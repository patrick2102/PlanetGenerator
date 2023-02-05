//
// Created by Patrick on 28/04/2022.
//

#include "CubeSphere.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SUN_H
#define ITU_GRAPHICS_PROGRAMMING_SUN_H

class Sun
{
public:
    Sun(glm::vec3 position, StarData starData)
            : starData(starData) {
        this->center = position;
        SetUpBuffers();
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

        //auto vertices = material.points;
        auto vertices = starData.atmosphere.points;

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
    StarData starData;

    void SetUpBuffers()
    {
        auto vertices = starData.atmosphere.points;
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
};
#endif //ITU_GRAPHICS_PROGRAMMING_SUN_H