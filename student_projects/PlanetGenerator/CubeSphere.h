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

const int cubeDivisions = 1;

class CubeSphere
{
public:
	float pi = glm::pi<float>();
	Shader* shader;
	float radius;
	std::vector<glm::vec3> vertices;

	CubeSphere(float radius, int divisions, Shader* shader)
	{
		this->radius = radius;
		this->shader = shader;
		CreateCubeSphere();
	}

	void Draw()
	{
		glm::mat4 model = glm::mat4(1.0f);

		shader->use();
	}

private:
	void CreateCubeSphere()
	{
		float latAngle;
		float lonAngle;

		int pointsNum = (int)pow(2, cubeDivisions) + 1;

		glm::vec3 points[cubeDivisions * cubeDivisions + 1][cubeDivisions * cubeDivisions + 1];

		for (int i = 0; i < pointsNum; i++)
		{
			latAngle = (pi / 4 - pi / 2 * (float)i) / ((float)pointsNum - 1);
			glm::vec3 latNormal = glm::vec3(glm::cos(latAngle), -glm::sin(latAngle), 0);
			for (int j = 0; j < pointsNum; j++)
			{
				lonAngle = (pi / 4 - pi / 2 * (float)j) / ((float)pointsNum - 1);
				glm::vec3 lonNormal = glm::vec3(glm::cos(lonAngle), 0, -glm::sin(lonAngle));


				glm::vec3 v_dir = glm::normalize(glm::cross(latNormal, lonNormal));

				points[j][i] = v_dir;
			}
		}

		for (int y = 0; y < (pointsNum - 1); y++)
		{
			for (int x = 0; x < (pointsNum - 1); x++)
			{
				int index1 = x + (y + 1) * pointsNum;

				glm::vec3 p1 = points[x][y + 1];
				glm::vec3 p2 = points[x][y];
				glm::vec3 p3 = points[x + 1][y + 1];
				glm::vec3 p4 = points[x][y + 1];

				vertices.insert(vertices.end(), { p1, p2, p3 });
				vertices.insert(vertices.end(), { p3, p1, p4 });
			}
		}

	}
};

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H