//
// Created by Patrick on 28/04/2022.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
#define ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H

#include <glm/glm.hpp>

class HeightMapGenerator
{
public:
    static const int ptSize = 255;
    int permTab[ptSize*2];
    glm::vec3 grad3[12]= {   glm::vec3(1,1,0), glm::vec3(-1,1,0), glm::vec3(1,-1,0), glm::vec3(-1,-1,0),
                                glm::vec3(1,0,1), glm::vec3(-1,0,1), glm::vec3(1,0,-1), glm::vec3(-1,0,-1),
                                glm::vec3(0,1,1), glm::vec3(0,-1,1), glm::vec3(0,1,-1), glm::vec3(0,-1,-1)};

    HeightMapGenerator(float seed)
    {
        GeneratePermutationTable(seed);
    }

    void CopyToShader(Shader *shader)
    {
        shader->setIntArray("permTab", ptSize*2, permTab);
        shader->setVec3Array("grad3", 12, grad3);
    }

    void GeneratePermutationTable(float seed)
    {
        srand (seed);

        for(int i = 0; i < ptSize*2; i++)
        {
            permTab[i] = rand() % 255;
        }
    }
};


#endif //ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
