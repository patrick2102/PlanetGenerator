//
// Created by Patrick on 28/04/2022.
//

#include <glm/glm.hpp>

#ifndef ITU_GRAPHICS_PROGRAMMING_MISC_H
#define ITU_GRAPHICS_PROGRAMMING_MISC_H

struct Material
{
    Material(glm::vec3 reflectionColor, float ambientReflectance, float diffuseReflectance,
             float specularReflectance, float specularExponent) {
        this->reflectionColor = reflectionColor;
        this->ambientReflectance = ambientReflectance;
        this->diffuseReflectance = diffuseReflectance;
        this->specularReflectance = specularReflectance;
        this->specularExponent = specularExponent;
    }

    glm::vec3 reflectionColor;
    float ambientReflectance;
    float diffuseReflectance;
    float specularReflectance;
    float specularExponent;
};

struct Light
{
    Light(glm::vec3 p, glm::vec3 lc, float li) {
        position = p;
        lightColor = lc;
        lightIntensity = li;
    }

    glm::vec3 position;
    glm::vec3 lightColor;
    float lightIntensity;
};

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
};

struct Displacement
{
    int scale; // Scale of terrain. Larger equals wider terrain features.

    float amplitude; // Height of terrain. Larger equals taller terrain features.

    float persistence; //

    float lacunarity;

    int diameter;

    int iterations;

};

Material sunMaterial = Material(glm::vec3(1.0f), 1.0f, 0.0f, 0.0f, 1.0f);
Material planetMaterial = Material(glm::vec3(0.50f, 0.25f, 0.10f), 0.25f, 0.25f, 0.0f, 0.1f);


#endif //ITU_GRAPHICS_PROGRAMMING_MISC_H
