//
// Created by Patrick on 28/04/2022.
//

#ifndef ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H
#define ITU_GRAPHICS_PROGRAMMING_HEIGHTMAPGENERATOR_H

#include <stdlib.h>
#include <glm/glm.hpp>
#include <time.h>
#include <cmath>

class HeightMapGenerator
{
public:
    const char* folder = "planetNoise/";
    static const int ptSize = 255;
    int permTab[ptSize*2];
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
            permTab[i] = rand() % 255;
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

        int ii = i & ptSize;
        int jj = j & ptSize;

        int gi0 = permTab[ii+permTab[jj]] % 12;
        int gi1 = permTab[ii+i1+permTab[jj+j1]] % 12;
        int gi2 = permTab[ii+1+permTab[jj+1]] % 12;

        double x1 = x0 - i1 + G2;
        double y1 = y0 - j1 + G2;
        double x2 = x0 - 1.0 + 2.0 * G2;
        double y2 = y0 - 1.0 + 2.0 * G2;

        int giArr[3] = {gi0, gi1, gi2};
        double xArr[3] = {x0, x1, x2};
        double yArr[3] = {y0, y1, y2};

        double n = 0;

        for(int k = 0; k < 3; k++)
        {
            double _t = 0.5 - xArr[k]*xArr[k] - yArr[k]*yArr[k];
            if(_t >= 0)
            {
                _t *= _t;
                n += _t * _t * glm::dot(grad3[giArr[k]], glm::vec3(xArr[k], yArr[k], 0));
            }
        }

        return 70.0 * n;
    }

    double SimplexNoise3D(double x, double y, double z)
    {
        double F3 = 1.0/3.0;
        double G3 = 1.0/6.0;

        double s = (x+y+z)*F3;
        int i = floor(x+s);
        int j = floor(y+s);
        int k = floor(z+s);

        double t = (i+j+k)*G3;
        double x0 = x - (i-t);
        double y0 = y - (j-t);
        double z0 = z - (k-t);

        int i1, j1, k1;
        int i2, j2, k2;
        if(x0>=y0) {
            if(y0>=z0)
            { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
            else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
            else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
        }
        else { // x0<y0
            if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
            else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
            else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
        }


        int ii = i & ptSize;
        int jj = j & ptSize;
        int kk = k & ptSize;

        int gi0 = permTab[ii+permTab[jj+permTab[kk]]] % 12;
        int gi1 = permTab[ii+i1+permTab[jj+j1+permTab[kk+k1]]] % 12;
        int gi2 = permTab[ii+i2+permTab[jj+j2+permTab[kk+k2]]] % 12;
        int gi3 = permTab[ii+1+permTab[jj+1+permTab[kk+1]]] % 12;

        double x1 = x0 - i1 + G3;
        double y1 = y0 - j1 + G3;
        double z1 = z0 - k1 + G3;
        double x2 = x0 - i2 + 2.0 * G3;
        double y2 = y0 - j2 + 2.0 * G3;
        double z2 = z0 - k2 + 2.0 * G3;
        double x3 = x0 - 1.0 + 3.0 * G3;
        double y3 = y0 - 1.0 + 3.0 * G3;
        double z3 = z0 - 1.0 + 3.0 * G3;

        int giArr[4] = {gi0, gi1, gi2, gi3};
        double xArr[4] = {x0, x1, x2, x3};
        double yArr[4] = {y0, y1, y2, y3};
        double zArr[4] = {z0, z1, z2, z3};

        double n = 0;

        for(int c = 0; c < 4; c++)
        {
            double _t = 0.5 - xArr[c]*xArr[c] - yArr[c]*yArr[c] - zArr[c]*zArr[c];
            if(_t >= 0)
            {
                _t *= _t;
                n += _t * _t * glm::dot(grad3[giArr[c]], glm::vec3(xArr[c], yArr[c], zArr[c]));
            }
        }

        return 32.0 * n;
    }

    double** GenerateMap(int w, int h, int iterations, int scale, double amplitude, double persistence, double lacunarity)
    {
        scale += 1;

        double** heightMap = new double*[w];

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
    std::vector<double**> GenerateCubeMap(int w, int h, int iterations, int scale, double amplitude, double persistence, double lacunarity, int overlap)
    {
        scale += 1;
        std::vector<double**> sides(6);

        for(int side = 0; side < sides.size(); side++)
        {
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
        }
        return sides;
    }
  */

    void GenerateCubeMapSide(int s1, int s2, int s3, int h, int w, int d, double scale, double*** heightMap, double amplitude)
    {
        int i;
        int j;
        int k = d;

        double x = (double) s1 / scale;
        double y = (double) s2 / scale;
        double z = (double) s3 / scale;

        glm::vec3 point = glm::normalize(glm::vec3 (x,y,z));

        // Positive Z:
        for (i = 0; i < w; i++) {
            for (j = 0; j < h; j++) {
                for (k = 0; k < d; k++)
                {
                    double x = (double) i / scale;
                    double y = (double) j / scale;
                    double z = k;

                    glm::vec3 point = glm::normalize(glm::vec3 (x,y,z));

                    heightMap[i][j][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                }
            }
        }


    }

    std::vector<double**> GenerateCubeMap(int w, int h, int d, float r, int iterations, int scale, double amplitude, double persistence, double lacunarity) {
        //scale += 1;

        //double ***heightMap = new double **[w];
        //std::vector<std::vector<std::vector<double>>> sides(6);
        int nSides = 6;
        std::vector<double**> sides;


        for(int i = 0; i < nSides; i++)
        {
            //sides.at(i) = std::vector<std::vector<double>>(w);
            //sides.at(i) = (double*);
            //double** heightMap = (double**)malloc(sizeof(double*)*w);
            double** heightMap = new double*[w];
            for(int j = 0; j < w; j++)
            {
                heightMap[j] = new double[h];
                //heightMap[j] = (double*)malloc(sizeof(double)*h);
                //memset(heightMap[j],255,sizeof(double)*h);
                for(int k = 0; k < h; k++)
                {
                    heightMap[j][k] = 0.0;
                }
            }
            sides.push_back(heightMap);
        }

        for(int iter = 0; iter < iterations; iter++)
        {

            int i;
            int j;
            int k;
            int counter = 0;

            // Positive X:
            i = w-1;
            for (j = 0; j < h; j++) {
                for (k = 0; k < d; k++) {
                    double x = (i/2);
                    double y = ((double)(j - (h/2)));
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += (w);
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(0)[k][j] += value;

                    //if(k == d-1 && j == 0 || k == 0 && j == 0)
                    //    std::cout << "pos x: {" << point.x << ", " << point.y << ", " << point.z << "} = " << value << " at iteration: " << counter << std::endl;
                    counter++;
                }
            }

            // Negative X:
            i = w-1;
            for (j = 0; j < h; j++) {
                for (k = 0; k < d; k++) {
                    double x = -(i/2);
                    double y = ((double)(j - (h/2)));
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += (w);
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(1)[k][j] += value;

                    //sides.at(1)[k][j] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(1)[k][j] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(1)[k][j] += SimplexNoise(point.z, point.y) * amplitude;
                }
            }
            // Positive Y:
            j = h-1;
            for (i = 0; i < w; i++) {
                for (k = 0; k < d; k++) {

                    double x = (double)(i - (w/2));
                    double y = j/2;
                    double z = (double)(k - (d/2));

                    /*
                    double x = ((double)(i - (w/2)))/(double) scale;
                    double y = (j/2)/(double) scale;
                    double z = ((double)(k - (d/2)))/(double) scale;
                    */

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += (w);
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(2)[i][k] += value;

                    //sides.at(2)[i][k] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(2)[i][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(2)[i][k] += SimplexNoise(point.x, point.z) * amplitude;
                }
            }

            // Negative Y:
            j = h-1;
            for (i = 0; i < w; i++) {
                for (k = 0; k < d; k++) {

                    double x = ((double)(i - (w/2)));
                    double y = -(j/2);
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += (w);
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(3)[i][k] += value;

                    //sides.at(3)[i][k] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(3)[i][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(3)[i][k] += SimplexNoise(point.x, point.z) * amplitude;
                }
            }

            // Positive Z
            k = d-1;
            for (j = 0; j < h; j++) {
                for (i = 0; i < w; i++) {
                    //double x = (double) i / (double) scale;
                    double x = ((double)(i - (w/2)));
                    double y = ((double)(j - (h/2)));
                    double z = (k/2);

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += (w);
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(4)[i][j] += value;

                    counter++;

                    //if(i == w-1 && j == 0 || i == 0 && j == 0)
                    //    std::cout << "pos z: {" << point.x << ", " << point.y << ", " << point.z << "} = " << value << " at iteration: " << counter << std::endl;
                }
            }

            // Negative Z:
            k = d-1;
            for (i = 0; i < w; i++) {
                for (j = 0; j < h; j++) {
                    double x = ((double)(i - (w/2)));
                    double y = ((double)(j - (h/2)));
                    double z = -(k/2);

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)w;
                    point += w;
                    point /= (float)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(5)[i][j] += value;

                    //sides.at(5)[i][j] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(5)[i][j] += SimplexNoise(point.x, point.y) * amplitude;

                }
            }
            scale /= lacunarity;
            amplitude *= persistence;
            std::cout << "Generating cube map iteration: " << iter << std::endl;
        }

        return sides;
    }

    std::string OutputImageFloat(int w, int h, double** heightMap, const char* fileName)
    {
        FILE *f;
        float *img = NULL;
        int filesize = 54 + w*h*sizeof(float);

        img = (float*)malloc(w*h*sizeof(float)*3);
        memset(img,0,w*h*sizeof(float)*3);

        int x;
        int y;
        float s;

        float max = -1000000, min = 1000000;

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

                s = heightMap[i][j];

                img[(x+y*w)] = (float)(s);
            }
        }

        unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
        unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 32,0};
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

        std::string outputFile = folder;
        outputFile.append("f_");
        outputFile.append(fileName);

        f = fopen(outputFile.c_str(),"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<h; i++)
        {
            float *ptr = img+(w*(h-i-1));
            fwrite(ptr, sizeof(float), w, f);
            //fwrite(bmppad,1,(4-(w*3)%4)%4,f);
        }

        free(img);
        fclose(f);
        return outputFile;
    }

    std::string OutputImage(int w, int h, double** heightMap, const char* fileName)
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
                auto val = heightMap[i][j];

                if(val > max)
                    max = val;
                if(min > val)
                    min = val;
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

                s = heightMap[i][j] + min;

                s = floor(s);

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

        std::string outputFile = folder;
        outputFile.append("b_");
        outputFile.append(fileName);

        f = fopen(outputFile.c_str(),"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<h; i++)
        {
            fwrite(img+(w*(h-i-1)*3),3,w,f);
            fwrite(bmppad,1,(4-(w*3)%4)%4,f);
        }

        free(img);
        fclose(f);
        return outputFile;
    }

    std::string OutputImage2(int w, int h, double** heightMap, const char* fileName)
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
                auto val = heightMap[i][j];

                if(val > max)
                    max = val;
                if(min > val)
                    min = val;
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

                s = heightMap[i][j] + min;

                s = floor(s);

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

        std::string outputFile = folder;
        outputFile.append("b_");
        outputFile.append(fileName);

        f = fopen(outputFile.c_str(),"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<h; i++)
        {
            fwrite(img+(w*(h-i-1)*3),3,w,f);
            fwrite(bmppad,1,(4-(w*3)%4)%4,f);
        }

        free(img);
        fclose(f);
        return outputFile;
    }

    std::vector<std::string> OutputCubeMapImage(int w, int h, int d, std::vector<double**> sides, const char* fileName)
    {
        std::vector<std::string> fileNames;

        std::string outputFileName= "cube/PosX_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, h, sides.at(0), outputFileName.c_str()));
        outputFileName= "cube/NegX_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, h, sides.at(1), outputFileName.c_str()));


        outputFileName= "cube/PosY_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(w, d, sides.at(2), outputFileName.c_str()));
        outputFileName= "cube/NegY_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(w, d, sides.at(3), outputFileName.c_str()));


        outputFileName= "cube/PosZ_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(w, h, sides.at(4), outputFileName.c_str()));
        outputFileName= "cube/NegZ_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(w, h, sides.at(5), outputFileName.c_str()));

        return fileNames;
    }

    void MakeVisualization(int w, int h, const char* fileName)
    {
        FILE *f;
        unsigned char *img = NULL;
        int headerOffset = 54;
        int filesize = 54 + w*h*3;  //w is your image width, h is image height, both int

        img = (unsigned char *)malloc(3*w*h);
        memset(img,0,3*w*h);

        int x;
        int y;
        double s;

        float** heightMap = new float*[h];

        f = fopen(fileName,"r");

        unsigned char bmppad[3] = {0,0,0};

        fseek (f , headerOffset, SEEK_SET);
        //float buffer[w];

        for(int i=0; i<h; i++)
        {
            heightMap[i] = new float[w];
            fread(heightMap[i], sizeof(float), w, f);
        }

        float max = -1000000, min = 1000000;

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


        std::string outputFile = fileName;
        outputFile.append("_v_.bmp");

        f = fopen(outputFile.c_str(),"wb");
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
