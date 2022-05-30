//
// Created by Patrick on 28/04/2022.
//

#include <glm/glm.hpp>
#include "vector"


#ifndef ITU_GRAPHICS_PROGRAMMING_MISC_H
#define ITU_GRAPHICS_PROGRAMMING_MISC_H

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // Color
    glm::vec3 Color;
};


struct Material
{
    Material( std::string name, glm::vec3 reflectionColor, float diffuseReflectance, float roughness, float metalness) {
        this->name = name;
        this->reflectionColor = reflectionColor;
        this->diffuseReflectance = diffuseReflectance;
        this->roughness = roughness;
        this->metalness = metalness;
    }
    std::string name;
    unsigned int VAO;
    std::vector<Vertex> points;

    glm::vec3 reflectionColor;
    float diffuseReflectance;
    float roughness;
    float metalness;
};

struct StarMaterial
{
    StarMaterial(glm::vec3 reflectionColor, float ambientReflectance, float diffuseReflectance,
    float specularReflectance, float specularExponent, float roughness, float metalness, std::string name) {
        this->reflectionColor = reflectionColor;
        this->ambientReflectance = ambientReflectance;
        this->diffuseReflectance = diffuseReflectance;
        this->specularReflectance = specularReflectance;
        this->specularExponent = specularExponent;
        this->roughness = roughness;
        this->metalness = metalness;
        this->name = name;
    }

    unsigned int VAO;
    std::vector<Vertex> points;

    glm::vec3 reflectionColor;
    float ambientReflectance;
    float diffuseReflectance;
    float specularReflectance;
    float specularExponent;
    std::string name;

    float roughness;
    float metalness;
};

struct Light
{
    Light(glm::vec3 p, glm::vec3 lc, float li, float lr) {
        position = p;
        lightColor = lc;
        lightIntensity = li;
        lightRadius = lr;
    }

    glm::vec3 position;
    glm::vec3 lightColor;
    float lightIntensity;
    float lightRadius;
};

struct Ocean
{
    Ocean(Material material) : material(material)
    {
        this->material = material;
    }
    Material material = material;
};

struct Displacement
{
    Displacement(int scale, float amplitude, float persistence,
    float lacunarity, int diameter, int iterations) {
        this->scale = scale;
        this->amplitude = amplitude;
        this->persistence = persistence;
        this->lacunarity = lacunarity;
        this->diameter = diameter;
        this->iterations = iterations;
    }

    int scale; // Scale of terrain. Larger equals wider terrain features.
    float amplitude; // Height of terrain. Larger equals taller terrain features.
    float persistence; //
    float lacunarity;
    int diameter;
    int iterations;
};

struct StarData
{
    StarData(Light light, Displacement displacement, StarMaterial material)
        : light(light), displacement(displacement), material(material)
    {
        this->light = light;
        this->displacement = displacement;
        this->material = material;
    }
    Light light;
    Displacement displacement;
    StarMaterial material;

};

struct PlanetData
{
    PlanetData(std::vector<Material> materials, Displacement displacement, Ocean ocean) : displacement(displacement), ocean(ocean) {
        this->materials = materials;
        this->displacement = displacement;
        this->ocean = ocean;
    }
    std::vector<Material> materials;
    Ocean ocean;
    Displacement displacement;
};

//Material types:
StarMaterial sunMaterial = StarMaterial(glm::vec3(1.0f), 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, "sun");
//Material sunMaterial = Material("sun", glm::vec3(1.0f), 1.0f, 0.0f, 0.0f);
Material planetMaterial = Material("planet", glm::vec3(1.0f), 0.25f, 1.0f, 0.0f);

//TODO should be randomized
int scale = 50;
float amplitude = 25.0f;
float persistence = 0.5f;
float lacunarity = 2.0f;
int diameter = 100;
int iterations = 20;

Displacement testDisplacement = Displacement(scale, amplitude, persistence, lacunarity, diameter, iterations);

std::vector<Material> planetMaterials {planetMaterial};

glm::vec3 pos = glm::vec3(0.0f);
glm::vec3 color = glm::vec4(0.9f);
float intensity = 10.0f;
float lightRadius = 10.0f;
Light light = Light(pos, color, intensity, lightRadius);

Displacement sunDisplacement = Displacement(scale, amplitude, persistence, lacunarity, diameter, 0);
StarData starData = StarData(light, sunDisplacement, sunMaterial);

#endif //ITU_GRAPHICS_PROGRAMMING_MISC_H
