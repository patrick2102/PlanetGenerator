//
// Created by Patrick on 29/05/2022.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_BIOMEGENERATOR_H
#define ITU_GRAPHICS_PROGRAMMING_BIOMEGENERATOR_H

#include <vector>
#include <glm/glm.hpp>
#include "Misc.h"

struct PlanetType
{
    PlanetType(std::vector<Material> materialsAllowed)
    {
        this->materialsAllowed = materialsAllowed;
    }
    std::vector<Material> materialsAllowed;
};

Material dirtMaterial = Material(glm::vec3(0.4, 0.1, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material rockMaterial = Material(glm::vec3(0.5, 0.4, 0.4f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material iceMaterial = Material(glm::vec3(0, 0, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material grassMaterial = Material(glm::vec3(0, 0.8, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material desertMaterial = Material(glm::vec3(1.0, 0.9, 0.7f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);

Material redMaterial = Material(glm::vec3(1.0, 0, 0), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material greenMaterial = Material(glm::vec3(0.0, 1.0, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);
Material blueMaterial = Material(glm::vec3(0.0, 0.9, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f);


//PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, iceMaterial, grassMaterial, desertMaterial});
//PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, grassMaterial, desertMaterial});
PlanetType earthLike = PlanetType(std::vector<Material>{redMaterial, greenMaterial, blueMaterial});

class BiomeGenerator {
public:

    const char* folder = "voronoi/";

    BiomeGenerator()
    {

    }

    std::vector<glm::vec3> DistributePointsRandom(float seed, int nCells, float radius)
    {
        std::vector<glm::vec3> points;
        srand (seed);
        for(int i = 0; i < nCells; i++)
        {
            float x = (rand() % 2000) + (-1000);
            float y = (rand() % 2000) + (-1000);
            float z = (rand() % 2000) + (-1000);

            auto point = glm::vec3(x, y, z);
            point = glm::normalize(point) * radius;
            points.push_back(point);
        }
        return points;
    }
/*
    std::vector<glm::vec3> DistributePointsEqually(float seed, int nCells, float radius)
    {
        float pi = glm::pi<float>();
        std::vector<glm::vec3> points;

        float a1 = pi/2;

        float xRot = sqrt(nCells);
        float yRot = sqrt(nCells)/2;
        float increment = pi * (1.0f/(float)xRot);

        for(int j = 0; j < xRot; j++)
        {
            for(int i = 0; i < yRot*2; i++)
            {
                float a2 = pi * ((float)i)/(float)xRot;

                glm::vec3 point = GetPointOnSphere(a1, a2, radius);
                point = glm::normalize(point) * radius;
                points.push_back(point);
            }
            a1 -= increment;
        }
        return points;
    }
    */

    int NearestPoint(glm::vec3 coord, std::vector<glm::vec3> voronoiPoints)
    {
        float maxDist = 999999999.0f;
        int nearestIndex;
        for(int i = 0; i < voronoiPoints.size(); i++)
        {
            glm::vec3 voronoiPoint = voronoiPoints[i];
            float dist = glm::distance(voronoiPoint, coord);

            if(dist < maxDist)
            {
                maxDist = dist;
                nearestIndex = i;
            }
        }
        return nearestIndex;
    }

    int NearestPointAverage(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::vector<glm::vec3> voronoiPoints)
    {
        float maxDist = 999999999.0f;
        int nearestIndex;
        for(int i = 0; i < voronoiPoints.size(); i++)
        {
            glm::vec3 voronoiPoint = voronoiPoints[i];
            float dist1 = glm::distance(voronoiPoint, p1);
            float dist2 = glm::distance(voronoiPoint, p2);
            float dist3 = glm::distance(voronoiPoint, p3);
            float dist = (dist1+dist2+dist3)/3;

            if(dist < maxDist)
            {
                maxDist = dist;
                nearestIndex = i;
            }
        }
        return nearestIndex;
    }

    std::vector<Material> CreateMaterialsNaive(float seed, int nCells, float radius, std::vector<Vertex> coords, PlanetType pt)
    {
        auto voronoiPoints = DistributePointsRandom(seed, nCells, radius);
        std::vector<Material> materials;

        /*
        Material material = planetMaterial;

        for(unsigned int i = 0; i < coords.size(); i++)
        {
            material.points.push_back(coords[i]);
        }

        materials.push_back(material);
        */

        srand(time(NULL));

        for(int i = 0; i < nCells; i++)
        {
            int index = rand()%pt.materialsAllowed.size();

            Material am = pt.materialsAllowed[index];

            Material material = Material(am.reflectionColor, am.ambientReflectance, am.diffuseReflectance,
                                         am.specularReflectance, am.specularExponent, am.roughness, am.metalness);

            materials.push_back(material);
        }

        for(int i = 0; i < coords.size(); i +=3)
        {
            //int pIndex = NearestPoint(coords[i].Position, voronoiPoints);

            auto p1 = coords[i].Position;
            auto p2 = coords[i+1].Position;
            auto p3 = coords[i+2].Position;

            int pIndex = NearestPointAverage(p1, p2, p3, voronoiPoints);

            materials[pIndex].points.push_back(coords[i]);
            materials[pIndex].points.push_back(coords[i+1]);
            materials[pIndex].points.push_back(coords[i+2]);
            //materials[pIndex].points.push_back(coords[i+1]);
            //materials[pIndex].points.push_back(coords[i+2]);
        }

        return materials;
    }

private:

};

#endif //ITU_GRAPHICS_PROGRAMMING_BIOMEGENERATOR_H
