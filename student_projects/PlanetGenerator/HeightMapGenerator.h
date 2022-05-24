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
    static const int ptSize = 255;
    int permutationTable[ptSize*2];
    glm::vec3 grad3[12]= {   glm::vec3(1,1,0), glm::vec3(-1,1,0), glm::vec3(1,-1,0), glm::vec3(-1,-1,0),
                                glm::vec3(1,0,1), glm::vec3(-1,0,1), glm::vec3(1,0,-1), glm::vec3(-1,0,-1),
                                glm::vec3(0,1,1), glm::vec3(0,-1,1), glm::vec3(0,1,-1), glm::vec3(0,-1,-1)};

    HeightMapGenerator(double seed)
    {
        GeneratePermutationTable(seed);
    }

    void GeneratePermutationTable(float seed)
    {
        srand (seed);

        for(int i = 0; i < ptSize*2; i++)
        {
            permutationTable[i] = rand() % 256;
        }
    }

    double SimplexNoise(double x, double y)
    {
        double F2 = (sqrt(3) - 1)/2;
        double G2 = (3.0 - sqrt(3.0))/6.0;

        double s = (x+y)*F2;
        int i = floor(x+s);
        int j = floor(y+s);

        double t = (i+j)*G2;
        double x0 = x - (i-t);
        double y0 = y - (j-t);

        int i1, j1;
        if(x0>y0)
        {
            i1=1; j1=0;
        }
        else
        {
            i1=0; j1=1;
        }

        double x1 = x0 - i1 + G2;
        double y1 = y0 - j1 + G2;
        double x2 = x0 - 1.0 + 2.0 * G2;
        double y2 = y0 - 1.0 + 2.0 * G2;

        int ii = i & ptSize;
        int jj = j & ptSize;

        int gi0 = permutationTable[ii+permutationTable[jj]] % 12;
        int gi1 = permutationTable[ii+i1+permutationTable[jj+j1]] % 12;
        int gi2 = permutationTable[ii+1+permutationTable[jj+1]] % 12; //Maybe here?

        /*
        const int gi0 = hash(i + hash(j));
        const int gi1 = hash(i + i1 + hash(j + j1));
        const int gi2 = hash(i + 1 + hash(j + 1));
        const int gi0 = hash(i + hash(j));
        const int gi1 = hash(i + i1 + hash(j + j1));
        const int gi2 = hash(i + 1 + hash(j + 1));
        */

        double n0, n1, n2;

        double t0 = 0.5 - x0*x0 - y0*y0;

        if(t0 < 0)
            n0 = 0.0;
        else
        {
            t0 *= t0;
            n0 = t0 * t0 * glm::dot(grad3[gi0], glm::vec3(x0, y0, 0));
        }

        double t1 = 0.5 - x1*x1 - y1*y1;
        if(t1 < 0)
            n1 = 0.0;
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * glm::dot(grad3[gi1], glm::vec3(x1, y1, 0));
        }

        double t2 = 0.5 - x2*x2 - y2*y2;
        if(t2 < 0)
            n2 = 0.0;
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * glm::dot(grad3[gi2], glm::vec3(x2, y2, 0));
        }

        return (n0 + n1 + n2);
    }
/*
    double** GenerateMap(int w, int h)
    {
        double** heightMap = new double*[h*scale];

        for(int i = 0; i < (w*scale); i++)
        {
            heightMap[i] = new double[w*scale];
        }


        for(int i = 0; i < (w*scale); i++)
        {
            for(int j = 0; j < (h*scale); j++)
            {
                double x = (double)i/(double)scale;
                double y = (double)j/(double)scale;
                heightMap[i][j] = SimplexNoise(x, y);

            }
        }
        return heightMap;
    }*/

    double** GenerateMap(int w, int h, int iterations, int scale, double amplitude, double persistence, double lacunarity)
    {
        scale += 1;

        double** heightMap = new double*[h];

        for(int i = 0; i < w; i++)
        {
            heightMap[i] = new double[h];
            for(int j = 0; j < h; j++)
            {
                heightMap[i][j] = 0.0; // just to make sure that every value in heightMap is 0 from the start.
            }
        }

        for (int iter = 0; iter < iterations; iter++)
        {
            for(int i = 0; i < w; i++)
            {
                for(int j = 0; j < h; j++)
                {
                    double x = (double)i/(double)scale;
                    double y = (double)j/(double)scale;
                    heightMap[i][j] += SimplexNoise(x, y) * amplitude;

                }
            }
            scale /= lacunarity;
            amplitude *= persistence;
        }
        return heightMap;
    }


    /*
    double Perlin(double x, double y, double z)
    {

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
     */

    void OutputImage(int w, int h, double** heightMap)
    {
        FILE *f;
        unsigned char *img = NULL;
        int filesize = 54 + w*h*3;  //w is your image width, h is image height, both int

        img = (unsigned char *)malloc(3*w*h);
        memset(img,0,3*w*h);

        int x;
        int y;
        double s;

        double max = -1000000, min = 1000000;

        for(int i=0; i<w; i++)
        {
            for(int j=0; j<h; j++)
            {
                if(heightMap[i][j] > max)
                    max = heightMap[i][j];
                if(min > heightMap[i][j])
                    min = heightMap[i][j];
            }
        }

        if(min < 0)
            min *= -1;

        for(int i=0; i<w; i++)
        {
            for(int j=0; j<h; j++)
            {
                x=i;
                y=(h-1)-j;

                s = ((heightMap[i][j] + min)/(max+min))*255;
                //std::cout << s << ", ";
                if (s > 255) s=255;

                img[(x+y*w)*3+2] = (unsigned char)(s);
                img[(x+y*w)*3+1] = (unsigned char)(s);
                img[(x+y*w)*3+0] = (unsigned char)(s);
            }
            // std::cout << std::endl;
        }

        unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
        unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
        unsigned char bmppad[3] = {0,0,0};

        bmpfileheader[ 2] = (unsigned char)(filesize    );
        bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
        bmpfileheader[ 4] = (unsigned char)(filesize>>16);
        bmpfileheader[ 5] = (unsigned char)(filesize>>24);

        bmpinfoheader[ 4] = (unsigned char)(       w    );
        bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       w>>16);
        bmpinfoheader[ 7] = (unsigned char)(       w>>24);
        bmpinfoheader[ 8] = (unsigned char)(       h    );
        bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
        bmpinfoheader[10] = (unsigned char)(       h>>16);
        bmpinfoheader[11] = (unsigned char)(       h>>24);

        f = fopen("img.bmp","wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<h; i++)
        {
            fwrite(img+(w*(h-i-1)*3),3,w,f);
            fwrite(bmppad,1,(4-(w*3)%4)%4,f);
        }

        free(img);
        fclose(f);
    }

private:

};


#endif //ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
