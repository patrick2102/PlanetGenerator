//
// Created by Patrick on 27/04/2022.
//

#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H
#define ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H

#include <shader.h>

//const int cubeDivisions = 0;

enum CubeSphereSide
        {
            positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ
        };

class CubeSphere
{
public:
	float pi = glm::pi<float>();
	Shader* shader;
	float radius;
	std::vector<glm::vec3> vertices;

	CubeSphere(float radius, int divisions)
	{
		this->radius = radius;
		CreateCubeSphere(divisions);
	}

	void Draw()
	{
		glm::mat4 model = glm::mat4(1.0f);

		shader->use();
	}

private:
	void CreateCubeSphere(int divisions)
	{
		float latAngle;
		float lonAngle;

		int pointsNum = (int)pow(2, divisions) + 1;

        const float DEG2RAD = acos(-1) / 180.0f;

        CubeSphereSide cubeSphereSide[] = {positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ};

        for(int sideNum = 0; sideNum < 6; sideNum++)
        {
            std::vector<std::vector<glm::vec3>> points(pointsNum, std::vector<glm::vec3>(pointsNum));

            for (int i = 0; i < pointsNum; ++i)
            {
                latAngle = DEG2RAD * (45.0f - 90.0f * i / (pointsNum - 1));
                glm::vec3 latNormal = GetLatitudeNormalForSide(cubeSphereSide[sideNum], latAngle);
                for (int j = 0; j < pointsNum; ++j)
                {
                    lonAngle = DEG2RAD * (-45.0f + 90.0f * j / (pointsNum - 1));
                    glm::vec3 lonNormal = GetLongitudeNormalForSide(cubeSphereSide[sideNum], lonAngle);

                    glm::vec3 v_dir = glm::normalize(glm::cross(lonNormal, latNormal));

                    points[j][i] = v_dir;
                }
            }

            for (int i = 0; i < (pointsNum - 1); i++)
            {
                for (int j = 0; j < (pointsNum - 1); j++)
                {
                    glm::vec3 p1 = points[i][j];
                    glm::vec3 p2 = points[i + 1][j];
                    glm::vec3 p3 = points[i][j + 1];
                    glm::vec3 p4 = points[i + 1][j + 1];

                    vertices.insert(vertices.end(), { p1, p2, p3 });
                    vertices.insert(vertices.end(), { p3, p2, p4 });
                }
            }
            /*
            std::cout << "printing points: " << std::endl;

            for (int i = 0; i < (pointsNum); i++)
            {
                for (int j = 0; j < (pointsNum); j++)
                {
                    std::cout << "P" << j+i*pointsNum << " = (" << points[j][i].x << ", " << points[j][i].y << ", "
                              << points[j][i].z << ")" << std::endl;
                }
            }
            */
            /*
            std::cout << "printing vectors: " << std::endl;
            for (int i = 0; i < vertices.size(); i++)
            {
                //std::cout << "v" << std::to_string(i) << " {" << std::to_string(vertices[i].x) << ", " <<
                //std::to_string(vertices[i].y) << ", " << std::to_string(vertices[i].z) << "}" << std::endl;
            }
             */
        }
	}

	glm::vec3 GetLatitudeNormalForSide(CubeSphereSide side, float latAngle)
    {
	    switch(side)
        {
            case positiveX:
                return glm::vec3(-sin(latAngle), glm::cos(latAngle), 0);
            case negativeX:
                return glm::vec3(-sin(latAngle), glm::cos(latAngle), 0);
            case positiveY:
                return glm::vec3(-sin(latAngle), 0, glm::cos(latAngle));
            default:
                return glm::vec3(0,0,0);
        }
    }
    glm::vec3 GetLongitudeNormalForSide(CubeSphereSide side, float lonAngle)
    {
        switch(side)
        {
            case positiveX:
                return glm::vec3(-glm::sin(lonAngle), 0, -glm::cos(lonAngle));
            case negativeX:
                return glm::vec3(glm::sin(lonAngle), 0, glm::cos(lonAngle));
            case positiveY:
                return glm::vec3(0, -glm::sin(lonAngle), -glm::cos(lonAngle));
            default:
                return glm::vec3(0,0,0);
        }
    }
};

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H