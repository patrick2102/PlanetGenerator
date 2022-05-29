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

Material dirtMaterial = Material(glm::vec3(0.4, 0.1, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "dirt");
Material rockMaterial = Material(glm::vec3(0.5, 0.4, 0.4f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "rock");
Material iceMaterial = Material(glm::vec3(0, 0, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "ice");
Material grassMaterial = Material(glm::vec3(0, 0.8, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "grass");
Material desertMaterial = Material(glm::vec3(1.0, 0.9, 0.7f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "desert");

Material redMaterial = Material(glm::vec3(1.0, 0, 0), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "red");
Material greenMaterial = Material(glm::vec3(0.0, 1.0, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "green");
Material blueMaterial = Material(glm::vec3(0.0, 0.9, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "blue");


//PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, iceMaterial, grassMaterial, desertMaterial});
PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, grassMaterial, desertMaterial});
//PlanetType earthLike = PlanetType(std::vector<Material>{redMaterial, greenMaterial, blueMaterial});
//PlanetType earthLike = PlanetType(std::vector<Material>{redMaterial, greenMaterial, blueMaterial});

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

    std::vector<int> NearestThreePoints(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::vector<glm::vec3> voronoiPoints)
    {
        float nearestDist = 999999999.0f;
        float secondNearestDist = 999999999.0f;
        float thirdNearestDist = 999999999.0f;
        int nearestIndex = 0;
        int secondNearestIndex = 0;
        int thirdNearestIndex = 0;
        for(int i = 0; i < voronoiPoints.size(); i++)
        {
            glm::vec3 voronoiPoint = voronoiPoints[i];
            float dist1 = glm::distance(voronoiPoint, p1);
            float dist2 = glm::distance(voronoiPoint, p2);
            float dist3 = glm::distance(voronoiPoint, p3);
            float dist = (dist1+dist2+dist3)/3;

            if(dist < nearestDist)
            {
                nearestDist = dist;
                nearestIndex = i;
                continue;
            }
            if(dist < secondNearestDist)
            {
                secondNearestDist = dist;
                secondNearestIndex = i;
                continue;
            }
            if(dist < thirdNearestDist)
            {
                thirdNearestDist = dist;
                thirdNearestIndex = i;
                continue;
            }
        }

        std::vector<int> nearestIndices = {nearestIndex, secondNearestIndex, thirdNearestIndex};

        return nearestIndices;
    }

    std::vector<Material> SetupMaterials(int nCells, PlanetType pt)
    {
        std::vector<Material> materials;

        srand(time(NULL));

        for(int i = 0; i < nCells; i++)
        {
            int index = rand()%pt.materialsAllowed.size();

            Material am = pt.materialsAllowed[index];

            Material material = Material(am.reflectionColor, am.ambientReflectance, am.diffuseReflectance,
                                         am.specularReflectance, am.specularExponent, am.roughness, am.metalness, am.name);

            materials.push_back(material);
        }

        return materials;
    }

    std::tuple<std::vector<Vertex>, int> AssignMaterialsMix(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::vector<Material> materials, std::vector<glm::vec3>voronoiPoints)
    {
        /*
        auto nearestPoints = NearestThreePoints(p1, p2, p3, voronoiPoints);

        Material m1 = materials[nearestPoints[0]];
        Material m2 = materials[nearestPoints[1]];
        Material m3 = materials[nearestPoints[2]];

        auto combinedDist1 = glm::distance(p1, voronoiPoints[0])+glm::distance(p1, voronoiPoints[1])+glm::distance(p1, voronoiPoints[2]);
        auto combinedDist2 = glm::distance(p2, voronoiPoints[0])+glm::distance(p2, voronoiPoints[1])+glm::distance(p2, voronoiPoints[2]);
        auto combinedDist3 = glm::distance(p3, voronoiPoints[0])+glm::distance(p3, voronoiPoints[1])+glm::distance(p3, voronoiPoints[2]);


        auto n1 = glm::normalize(p1);
        auto c1 = glm::mix(m1.reflectionColor, m2.reflectionColor, glm::distance(p1, voronoiPoints[0])/combinedDist1);
        c1 = glm::mix(c1, m3.reflectionColor, glm::distance(p1, voronoiPoints[0])/combinedDist1);


        auto n2 = glm::normalize(p2);
        auto c2 = glm::mix(m1.reflectionColor, m2.reflectionColor, glm::distance(p2, voronoiPoints[0])/combinedDist2);
        c2 = glm::mix(c2, m3.reflectionColor, glm::distance(p2, voronoiPoints[0])/combinedDist2);

        auto n3 = glm::normalize(p3);
        auto c3 = glm::mix(m1.reflectionColor, m2.reflectionColor, glm::distance(p3, voronoiPoints[0])/combinedDist3);
        c3 = glm::mix(c3, m3.reflectionColor, glm::distance(p3, voronoiPoints[0])/combinedDist3);

        */

        auto nearestPoints = NearestThreePoints(p1, p2, p3, voronoiPoints);
        int pIndex = nearestPoints[0];

        auto v1 = Vertex{p1, p1, p1};
        auto v2 = Vertex{p2, p2, p2};
        auto v3 = Vertex{p3, p3, p3};

        std::vector<Vertex> vertices{v1, v2, v3};

        return std::make_tuple(vertices, pIndex);
    }


    std::vector<Material> CreateMaterialsNaive(float seed, int nCells, float radius, std::vector<Vertex> coords, PlanetType pt)
    {
        auto voronoiPoints = DistributePointsRandom(seed, nCells, radius);
        std::vector<Material> materials;

        srand(time(NULL));

        for(int i = 0; i < nCells; i++)
        {
            int index = rand()%pt.materialsAllowed.size();

            Material am = pt.materialsAllowed[index];

            Material material = Material(am.reflectionColor, am.ambientReflectance, am.diffuseReflectance,
                                         am.specularReflectance, am.specularExponent, am.roughness, am.metalness, am.name);

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
