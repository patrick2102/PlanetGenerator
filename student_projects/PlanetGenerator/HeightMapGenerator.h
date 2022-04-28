//
// Created by Patrick on 28/04/2022.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
#define ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H

#include <stdlib.h>
#include <glm/glm.hpp>
#include <time.h>

class HeightMapGenerator
{
public:
    HeightMapGenerator(int lacunarity, float persistance)
    {
        this->lacunarity = lacunarity;
        this->persistance = persistance;
    }


    void PerlinNoiseForSphere(float radius, int divisions)
    {
        glm::vec2 *gradients = gradientVectors(divisions, divisions);



    }

    glm::vec2* gradientVectors(int w, int h)
    {
        glm::vec2 *gradients = (glm::vec2*) malloc(sizeof(glm::vec2)*w*h);

        srand (time(NULL));

        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                float x = (float)rand() / (float)rand();
                float y = (float)rand() / (float)rand();
                glm::vec2 randVector = glm::normalize(glm::vec2(x, y));
                gradients[j + (i*h)] = randVector;
            }
        }
        return gradients;
    }

    float perlin(float x, float y)
    {
        // Determine grid cell coordinates
        int x0 = (int)floor(x);
        int x1 = x0 + 1;
        int y0 = (int)floor(y);
        int y1 = y0 + 1;

        // Determine interpolation weights
        // Could also use higher order polynomial/s-curve here
        float sx = x - (float)x0;
        float sy = y - (float)y0;

        // Interpolate between grid point gradients
        float n0, n1, ix0, ix1, value;

        n0 = dotGridGradient(x0, y0, x, y);
        n1 = dotGridGradient(x1, y0, x, y);
        ix0 = interpolate(n0, n1, sx);

        n0 = dotGridGradient(x0, y1, x, y);
        n1 = dotGridGradient(x1, y1, x, y);
        ix1 = interpolate(n0, n1, sx);

        value = interpolate(ix0, ix1, sy);
        return value;
    }

private:
    int lacunarity;
    float persistance;

};


#endif //ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
