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
};


struct Material
{
    Material(glm::vec3 reflectionColor, float ambientReflectance, float diffuseReflectance,
             float specularReflectance, float specularExponent, float roughness, float metalness) {
        this->reflectionColor = reflectionColor;
        this->ambientReflectance = ambientReflectance;
        this->diffuseReflectance = diffuseReflectance;
        this->specularReflectance = specularReflectance;
        this->specularExponent = specularExponent;
        this->roughness = roughness;
        this->metalness = metalness;
    }

    unsigned int VAO;
    std::vector<Vertex> points;

    glm::vec3 reflectionColor;
    float ambientReflectance;
    float diffuseReflectance;
    float specularReflectance;
    float specularExponent;

    float roughness;
    float metalness;
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
    StarData(Light light, Displacement displacement, Material material)
        : light(light), displacement(displacement), material(material)
    {
        this->light = light;
        this->displacement = displacement;
        this->material = material;
    }
    Light light;
    Displacement displacement;
    Material material;

};

/*
struct PlanetData
{
    PlanetData(Displacement surfaceDisplacement, Material material, Ocean ocean)
            : material(material), surfaceDisplacement(surfaceDisplacement), ocean(ocean) {
        this->surfaceDisplacement = surfaceDisplacement;
        this->material = material;
        this->ocean = ocean;
    }
    Displacement surfaceDisplacement;
    Material material;
    Ocean ocean;
};
*/

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
Material waterMaterial = Material(glm::vec3(0, 0, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);

Material sunMaterial = Material(glm::vec3(1.0f), 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
//Material planetMaterial = Material(glm::vec3(0.50f, 0.25f, 0.10f), 0.25f, 0.25f, 0.0f, 0.1f);
//Material planetMaterial = Material(glm::vec3(1.0f), 0.0f, 0.25f, 0.0f, 0.1f, 1.0f, 0.0f);
Material planetMaterial = Material(glm::vec3(1.0f), 0.0f, 0.25f, 0.0f, 0.1f, 1.0f, 0.0f);

Ocean planetOcean = Ocean(waterMaterial);

//TODO should be randomized
int scale = 50;
float amplitude = 25.0f;
float persistence = 0.5f;
float lacunarity = 2.0f;
int diameter = 100;
int iterations = 20;

Displacement testDisplacement = Displacement(scale, amplitude, persistence, lacunarity, diameter, iterations);

std::vector<Material> planetMaterials {planetMaterial};

PlanetData testPlanetData = PlanetData(planetMaterials, testDisplacement, planetOcean);

glm::vec3 pos = glm::vec3(0.0f);
glm::vec3 color = glm::vec4(0.9f);
float intensity = 1.0f;
Light light = Light(pos, color, intensity);

Displacement sunDisplacement = Displacement(scale, amplitude, persistence, lacunarity, diameter, 0);
StarData starData = StarData(light, sunDisplacement, sunMaterial);

#endif //ITU_GRAPHICS_PROGRAMMING_MISC_H
