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

		std::vector<std::vector<glm::vec3>> points(pointsNum, std::vector<glm::vec3>(pointsNum));
        //std::vector<glm::vec3> points;

		/*
		for (int i = 0; i < pointsNum; i++)
		{
            a1 = glm::radians(-45.0f + 90.0f * j / (pointsPerRow - 1));
            //lonAngle = (pi / 4 - pi / 2 * (float)i) / ((float)pointsNum - 1);
            glm::vec3 lonNormal = glm::vec3(-glm::sin(lonAngle), glm::cos(lonAngle), 0);
			for (int j = 0; j < pointsNum; j++)
			{
                latAngle = (pi / 4 - pi / 2 * (float)j) / ((float)pointsNum - 1);
                glm::vec3 latNormal = glm::vec3(-glm::sin(latAngle), 0, -glm::cos(latAngle));
                glm::vec3 v_dir = glm::normalize(glm::cross(latNormal, lonNormal));

                points[i][j] = v_dir;

			}
		}
		*/
		/*
        const float DEG2RAD = acos(-1) / 180.0f;

        std::vector<float> vertices;
        float n1[3];        // normal of longitudinal plane rotating along Y-axis
        float n2[3];        // normal of latitudinal plane rotating along Z-axis
        float v[3];         // direction vector intersecting 2 planes, n1 x n2

        float a1;           // longitudinal angle along Y-axis
        float a2;           // latitudinal angle along Z-axis

        // compute the number of vertices per row, 2^n + 1
        int pointsPerRow = (int)pow(2, divisions) + 1;

        for(unsigned int i = 0; i < pointsPerRow; ++i)
        {
            // normal for latitudinal plane
            // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
            // therefore, it is rotating (0,1,0) vector by latitude angle a2
            a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
            n2[0] = -sin(a2);
            n2[1] = cos(a2);
            n2[2] = 0;

            // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
            for(unsigned int j = 0; j < pointsPerRow; ++j)
            {
                // normal for longitudinal plane
                // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
                // therefore, it is rotating (0,0,-1) vector by longitude angle a1
                a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
                n1[0] = -sin(a1);
                n1[1] = 0;
                n1[2] = -cos(a1);

                // find direction vector of intersected line, n1 x n2
                v[0] = n1[1] * n2[2] - n1[2] * n2[1];
                v[1] = n1[2] * n2[0] - n1[0] * n2[2];
                v[2] = n1[0] * n2[1] - n1[1] * n2[0];

                // normalize direction vector
                float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
                v[0] *= scale;
                v[1] *= scale;
                v[2] *= scale;

                // add a vertex into array
                vertices.push_back(v[0]);
                vertices.push_back(v[1]);
                vertices.push_back(v[2]);
            }
        }
        */
        //std::cout << "printing angles: " << std::endl;
		for (int i = 0; i < pointsNum; ++i)
		{
			latAngle = DEG2RAD * (45.0f - 90.0f * i / (pointsNum - 1));
			glm::vec3 latNormal = glm::vec3(-sin(latAngle), glm::cos(latAngle), 0);
			for (int j = 0; j < pointsNum; ++j)
			{
				lonAngle = DEG2RAD * (-45.0f + 90.0f * j / (pointsNum - 1));
				glm::vec3 lonNormal = glm::vec3(-glm::sin(lonAngle), 0, -glm::cos(lonAngle));

                //std::cout << "angle lon: " << lonAngle  << ", angle lat: " << latAngle  <<std::endl;

				glm::vec3 v_dir = glm::normalize(glm::cross(lonNormal, latNormal));

				points[j][i] = v_dir; //Might switch around for performance reasons maybe?

                //points.insert(points.end(), v_dir);

				//vertices.insert(vertices.end(), {v_dir.x, v_dir.y, v_dir.z});
			}
		}

        /*
		for(int i = 0; i < points.size(); i++)
        {
		    vertices.insert(vertices.end(), points[i]);
            vertices.insert(points[i])
        }
		*/

        std::cout << "printing points: " << std::endl;

        for (int i = 0; i < (pointsNum); i++)
        {
            for (int j = 0; j < (pointsNum); j++)
            {
                std::cout << "P" << j+i*pointsNum << " = (" << points[j][i].x << ", " << points[j][i].y << ", "
                << points[j][i].z << ")" << std::endl;
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

        //std::cout << "printing vectors: " << std::endl;

        /*for (int i = 0; i < vertices.size(); i++)
        {
            //std::cout << "v" << std::to_string(i) << " {" << std::to_string(vertices[i].x) << ", " <<
            //std::to_string(vertices[i].y) << ", " << std::to_string(vertices[i].z) << "}" << std::endl;
        }
         */
	}
};

#endif //ITU_GRAPHICS_PROGRAMMING_CUBESPHERE_H