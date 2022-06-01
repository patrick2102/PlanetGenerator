//
// Created by Patrick on 29/05/2022.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_BIOMEGENERATOR_H
#define ITU_GRAPHICS_PROGRAMMING_BIOMEGENERATOR_H

#include <vector>
#include <limits>
#include <glm/glm.hpp>
#include "Misc.h"
using namespace std;

struct PlanetType
{
    PlanetType(std::vector<Material> materialsAllowed)
    {
        this->materialsAllowed = materialsAllowed;
    }
    std::vector<Material> materialsAllowed;
};
/*
Material redMaterial = Material(glm::vec3(1.0, 0, 0), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "red");
Material greenMaterial = Material(glm::vec3(0.0, 1.0, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "green");
Material blueMaterial = Material(glm::vec3(0.0, 0.9, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "blue");
*/

//Material waterMaterial = Material("water", glm::vec3(0.004, 0.086, 0.6f), 1.0f, 0.35f, 0.65f);
Material waterMaterial = Material("water", glm::vec3(0.004, 0.086, 0.2f), 1.0f, 0.25f, 0.5f);

/*
Material dirtMaterial = Material(glm::vec3(0.4, 0.1, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "dirt");
Material rockMaterial = Material(glm::vec3(0.5, 0.4, 0.4f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "rock");
Material iceMaterial = Material(glm::vec3(0, 0, 1.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "ice");
Material grassMaterial = Material(glm::vec3(0, 0.8, 0.0f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "grass");
Material desertMaterial = Material(glm::vec3(1.0, 0.9, 0.7f), 1.0f, 0.25f, 0.0f, 0.1f, 0.0f, 0.0f, "desert");
 */
Material jungleMaterial = Material("jungle", glm::vec3(0.13, 0.235, 0.043f), 1.0f, 0.0f, 0.0f);
Material temperateForestMaterial = Material("forest", glm::vec3(0.27, 0.325, 0.117f), 1.0f, 0.0f, 0.0f);
Material desertMaterial = Material("desert", glm::vec3(1.0, 0.9, 0.7f), 1.0f, 0.0f, 0.0f);
//Material iceMaterial = Material("ice", glm::vec3(0, 0, 1.0f), 1.0f, 0.0f, 0.0f);

//PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, iceMaterial, grassMaterial, desertMaterial});
//PlanetType earthLike = PlanetType(std::vector<Material>{dirtMaterial, rockMaterial, grassMaterial, desertMaterial});
PlanetType earthLike = PlanetType(std::vector<Material>{jungleMaterial, temperateForestMaterial, desertMaterial});

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

    std::vector<std::tuple<float, int>> NearestNPoints(tuple<glm::vec3, glm::vec3, glm::vec3> vertices, std::vector<glm::vec3> voronoiPoints, int n)
    {
        float max = numeric_limits<float>::max();

        std::vector<std::tuple<float, int>> nearestPoints;

        for(int i = 0; i < n; i++)
        {
            auto tuple = make_tuple(max, 0);
            nearestPoints.push_back(tuple);
        }

        for(int i = 0; i < voronoiPoints.size(); i++)
        {
            glm::vec3 voronoiPoint = voronoiPoints[i];
            float dist1 = glm::distance(voronoiPoint, get<0>(vertices));
            float dist2 = glm::distance(voronoiPoint, get<1>(vertices));
            float dist3 = glm::distance(voronoiPoint, get<2>(vertices));
            float dist = (dist1+dist2+dist3)/3;

            for(int j = 0; j < n; j++)
            {
                float nearestDist = get<0>(nearestPoints[j]);

                if(dist < nearestDist)
                {
                    for(int k = n-1; k > j; k--)
                    {
                        nearestPoints[k] = nearestPoints[k-1];
                    }
                    auto tuple = make_tuple(dist, i);
                    nearestPoints[j] = tuple;
                    break;
                }
            }
        }
        return nearestPoints;
    }

    std::vector<Material> SetupMaterials(int nCells, PlanetType pt)
    {
        std::vector<Material> materials;

        srand(time(NULL));

        for(int i = 0; i < nCells; i++)
        {
            int index = rand()%pt.materialsAllowed.size();

            Material am = pt.materialsAllowed[index];

            Material material = Material(am.name, am.reflectionColor, am.diffuseReflectance, am.roughness, am.metalness);

            materials.push_back(material);
        }

        return materials;
    }

    std::tuple<std::vector<Vertex>, int> AssignMaterialsMix(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::vector<Material> materials, std::vector<glm::vec3>voronoiPoints, int n)
    {
        auto pointsTuple = make_tuple(p1,p2,p3);

        auto nearestPoints = NearestNPoints(pointsTuple, voronoiPoints, n);


        auto n1 = glm::normalize(p1);
        auto n2 = glm::normalize(p2);
        auto n3 = glm::normalize(p3);

        float border = 0.2;
        float distToNearest = get<0>(nearestPoints[0]);

        Material m1 = materials[get<1>(nearestPoints[0])];
        glm::vec3 c = m1.reflectionColor;
        for(int i = 1; i < n; i++)
        {
            float distToOtherBiome = get<0>(nearestPoints[i]);
            if((distToNearest*(1+border)) > distToOtherBiome)
            {
                float combinedDist = distToNearest+distToOtherBiome;
                //float distDiff = dist1-dist0;
                float min = combinedDist * (0.50-border);
                float max = combinedDist * (0.50+border);
                float p = (distToNearest-min)/(max-min);

                c = glm::mix(c, materials[get<1>(nearestPoints[i])].reflectionColor, p);
            }
        }

        auto v1 = Vertex{p1, n1, c};
        auto v2 = Vertex{p2, n2, c};
        auto v3 = Vertex{p3, n3, c};

        std::vector<Vertex> vertices{v1, v2, v3};

        int pIndex = get<1>(nearestPoints[0]);
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

            Material material = Material(am.name, am.reflectionColor, am.diffuseReflectance, am.roughness, am.metalness);

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
