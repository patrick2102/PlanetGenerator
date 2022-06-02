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
	void CreateCubeSphere(int divisions, float r = 1)
	{
		float latAngle;
		float lonAngle;

		int pointsNum = (int)pow(2, divisions) + 1;

        CubeSphereSide cubeSphereSide[] = {positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ};

        int len = sizeof(cubeSphereSide)/sizeof(cubeSphereSide[0]);

        for(int sideNum = 0; sideNum < len; sideNum++)
        {
            std::vector<std::vector<glm::vec3>> points(pointsNum, std::vector<glm::vec3>(pointsNum));

            for (int i = 0; i < pointsNum; ++i)
            {
                latAngle = (0.25*pi - 0.5*pi * i/(pointsNum - 1));

                glm::vec3 latNormal = glm::vec3(-sin(latAngle), glm::cos(latAngle), 0);
                for (int j = 0; j < pointsNum; ++j)
                {
                    lonAngle = ((-0.25 * pi) + (0.5 * pi) * j / (pointsNum - 1));
                    glm::vec3 lonNormal = glm::vec3(-glm::sin(lonAngle), 0, -glm::cos(lonAngle));

                    glm::vec3 v_dir = glm::normalize(glm::cross(lonNormal, latNormal));

                    v_dir = RotateToSide(cubeSphereSide[sideNum], v_dir) * r;

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

                    vertices.insert(vertices.end(), { p2, p1, p3 });
                    vertices.insert(vertices.end(), { p3, p4, p2 });
                }
            }
        }
	}

	glm::vec3 RotateToSide(CubeSphereSide side, glm::vec3 vector)
    {
	    auto rotationMatrix = glm::mat4(1.0f);
	    auto q = glm::rotate(rotationMatrix, 0.0f, glm::vec3(0,1,0));
	    switch(side)
        {
            case positiveX:
                    return vector;
            case negativeX:
                return (glm::rotate(rotationMatrix, pi, glm::vec3(0,1,0)) * glm::vec4(vector, 1));
            case positiveY:
                return (glm::rotate(rotationMatrix, pi/2, glm::vec3(0,0,1)) * glm::vec4(vector, 1));
            case negativeY:
                return (glm::rotate(rotationMatrix, -pi/2, glm::vec3(0,0,1)) * glm::vec4(vector, 1));
            case positiveZ:
                return (glm::rotate(rotationMatrix, pi/2, glm::vec3(0,1,0)) * glm::vec4(vector, 1));
            case negativeZ:
                return (glm::rotate(rotationMatrix, -pi/2, glm::vec3(0,1,0)) * glm::vec4(vector, 1));
            default:
                return vector;
        }

    }

};

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H