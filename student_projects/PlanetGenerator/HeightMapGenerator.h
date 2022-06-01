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

    HeightMapGenerator(float seed)
    {
        GeneratePermutationTable(seed);
    }

    void CopyToShader(Shader *shader)
    {
        shader->setIntArray("permTab", ptSize*2, permTab);
        shader->setVec3Array("grad3", 12, grad3);
    }

    /*
    HeightMapGenerator(float seed)
    {
        GeneratePermutationTable(seed);
    }
    */

    unsigned int GenerateCubeMapUsingGPU(CubeSphere cubeSphere, PlanetData pd, Shader *shader)
    {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int d = pd.displacement.diameter;

        for (unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, d, d, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0,0,d,d);

        GLuint quad_vao;
        GLuint quad_vbo;
        glGenVertexArrays(1, &quad_vao);
        glBindVertexArray(quad_vao);

        static const GLfloat vertices[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f,  1.0f, 0.0f,
        };
        glGenBuffers(1, &quad_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteBuffers(1, &quad_vbo);

        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        GLubyte * pixels = new GLubyte[d*d*3];

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);

        glReadPixels(0, 0, d, d, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        //glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &pixels);

        for(int i = 0; i < d*d*3; i++)
        {
            std::cout << +pixels[i] << ", ";
        }



        /*
        float seed = 0.0f;
        auto hmg = new HeightMapGenerator(seed);

        auto sd = pd.displacement;

        hmg->CopyToShader(shader);
        */

        //std::vector<float **> heightCubeMap = hmg->GenerateCubeMap(sd.diameter, sd.iterations, sd.scale, sd.amplitude, sd.persistence, sd.lacunarity);
        //auto displacementFaces = hmg->OutputCubeMapImage(diameter, heightCubeMap, "aargh.bmp");

/*
        int width, height, nrComponents;
        for (unsigned int i = 0; i < displacementFaces.size(); i++)
        {
            unsigned char* data = stbi_load(displacementFaces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << displacementFaces[i] << std::endl;
                stbi_image_free(data);
            }
        }*/



        //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, DrawBuffers);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
        /*
        //glFrameBuff(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, DrawBuffers);
        //glViewport(0,0,d,d);

        //glBindVertexArray(0);
        unsigned int VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, cubeSphere.vertices.size() * sizeof(glm::vec3), &cubeSphere.vertices[0], GL_STATIC_DRAW);


        int posAttributeLocation = 0;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0);


        glDrawArrays(GL_TRIANGLES, 0, cubeSphere.vertices.size());

        //glBindVertexArray(0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        GLubyte* pixels = new GLubyte[d*d*3];
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X,  0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
*/
        /*
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_COLOR_ATTACHMENT0, GL_UNSIGNED_BYTE, pixels);
        glGetTexImage(GL_TEXTURE_CUBE_MAP, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
         */

        /*
        unsigned int framebuffers[6];
        for (unsigned int i = 0; i < 6; i++) {
            // Create framebuffer
            glGenFramebuffers(1, &framebuffers[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);

            //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            //glBindRe
            //glDrawBuffers(1, DrawBuffers);

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            // Attach one face of cube map
            //glFramebufferTexture2D(framebuffer, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
        }

        unsigned int VBO;

        const GLfloat vertices[] = {
                -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        shader->setInt("surfaceTexture", 0);
        shader->setVec3("tex_size", d-1, d-1, d-1);

        glBindVertexArray(0);
        //glDeleteBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

         */
        /*
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, cubeSphere.vertices.size() * sizeof(glm::vec3), &cubeSphere.vertices[0], GL_STATIC_DRAW);


        int posAttributeLocation = 0;
        glEnableVertexAttribArray(posAttributeLocation);
        glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0);

        glDrawArrays(GL_TRIANGLES, 0, cubeSphere.vertices.size());
        */
        /*
        for (unsigned int i = 0; i < 6; i++)
        {
            glGenFramebuffers(1, &framebuffers[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);

            GLubyte* pixels = new GLubyte[d*d*3];

            for(int j = 0; j < d*d*3; j++)
            {
                pixels[j] = 0;
            }

            glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, 0, d, d);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

            //glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            glReadPixels(0, 0, d, d, GL_RGB, GL_UNSIGNED_BYTE, pixels);

            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            //glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,  0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

            for(int j = 0; j < d*d*3; j++)
            {
                GLubyte val = pixels[j];
                std::cout << +val << ", ";
                if(j % 10 == 0)
                    std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            */
            /*
            for(int j = 0; j < d; j++)
            {
                for(int k = 0; k < d*3; k+=3)
                {
                    std::cout << "{ ";
                    for(int q = 0; q < 3; q++)
                    {
                        GLubyte val = pixels[k+(j*d)+q];
                        std::cout << +val << ", ";
                    }

                    std::cout << "}; ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cout << std::endl;
            */
            /*
            pixels;
        }
        */


        //glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, 0, d, d);

        return textureID;
    }

    void GeneratePermutationTable(float seed)
    {
        srand (seed);

        for(int i = 0; i < ptSize*2; i++)
        {
            permTab[i] = rand() % 255;
        }
    }

    float SimplexNoise(float x, float y)
    {
        float F2 = (sqrt(3) - 1)/2;
        float G2 = (3.0 - sqrt(3.0))/6.0;

        float s = (x+y)*F2;
        int i = floor(x+s);
        int j = floor(y+s);

        float t = (i+j)*G2;
        float x0 = x - (i-t);
        float y0 = y - (j-t);

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

        float x1 = x0 - i1 + G2;
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0 + 2.0 * G2;
        float y2 = y0 - 1.0 + 2.0 * G2;

        int giArr[3] = {gi0, gi1, gi2};
        float xArr[3] = {x0, x1, x2};
        float yArr[3] = {y0, y1, y2};

        float n = 0;

        for(int k = 0; k < 3; k++)
        {
            float _t = 0.5 - xArr[k]*xArr[k] - yArr[k]*yArr[k];
            if(_t >= 0)
            {
                _t *= _t;
                n += _t * _t * glm::dot(grad3[giArr[k]], glm::vec3(xArr[k], yArr[k], 0));
            }
        }

        return 70.0 * n;
    }

    float SimplexNoise3D(float x, float y, float z)
    {
        float F3 = 1.0/3.0;
        float G3 = 1.0/6.0;

        float s = (x+y+z)*F3;
        int i = floor(x+s);
        int j = floor(y+s);
        int k = floor(z+s);

        float t = (i+j+k)*G3;
        float x0 = x - (i-t);
        float y0 = y - (j-t);
        float z0 = z - (k-t);

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

        float x1 = x0 - i1 + G3;
        float y1 = y0 - j1 + G3;
        float z1 = z0 - k1 + G3;
        float x2 = x0 - i2 + 2.0 * G3;
        float y2 = y0 - j2 + 2.0 * G3;
        float z2 = z0 - k2 + 2.0 * G3;
        float x3 = x0 - 1.0 + 3.0 * G3;
        float y3 = y0 - 1.0 + 3.0 * G3;
        float z3 = z0 - 1.0 + 3.0 * G3;

        int giArr[4] = {gi0, gi1, gi2, gi3};
        float xArr[4] = {x0, x1, x2, x3};
        float yArr[4] = {y0, y1, y2, y3};
        float zArr[4] = {z0, z1, z2, z3};

        float n = 0;

        for(int c = 0; c < 4; c++)
        {
            float _t = 0.5 - xArr[c]*xArr[c] - yArr[c]*yArr[c] - zArr[c]*zArr[c];
            if(_t >= 0)
            {
                _t *= _t;
                n += _t * _t * glm::dot(grad3[giArr[c]], glm::vec3(xArr[c], yArr[c], zArr[c]));
            }
        }

        return 32.0 * n;
    }

    float** GenerateMap(int w, int h, int iterations, int scale, float amplitude, float persistence, float lacunarity)
    {
        scale += 1;

        float** heightMap = new float*[w];

        for(int i = 0; i < w; i++)
        {
            heightMap[i] = new float[h];
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
                    float x = (float)i/(float)scale;
                    float y = (float)j/(float)scale;
                    heightMap[i][j] += SimplexNoise(x, y) * amplitude;
                }
            }
            scale /= lacunarity;
            amplitude *= persistence;
        }
        return heightMap;
    }

    void GenerateCubeMapSide(int s1, int s2, int s3, int h, int w, int d, float scale, float *** heightMap, float amplitude)
    {
        int i;
        int j;
        int k = d;

        float x = (float ) s1 / scale;
        float y = (float ) s2 / scale;
        float z = (float ) s3 / scale;

        glm::vec3 point = glm::normalize(glm::vec3 (x,y,z));

        // Positive Z:
        for (i = 0; i < w; i++) {
            for (j = 0; j < h; j++) {
                for (k = 0; k < d; k++)
                {
                    float x = (float ) i / scale;
                    float y = (float ) j / scale;
                    float z = k;

                    glm::vec3 point = glm::normalize(glm::vec3 (x,y,z));

                    heightMap[i][j][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                }
            }
        }


    }

    std::vector<double**> GenerateCubeMap(int d, int iterations, int scale, double amplitude, double persistence, double lacunarity) {
        //scale += 1;

        //float ***heightMap = new float **[w];
        //std::vector<std::vector<std::vector<float >>> sides(6);
        int nSides = 6;
        std::vector<double**> sides;


        for(int i = 0; i < nSides; i++)
        {
            //sides.at(i) = std::vector<std::vector<float >>(w);
            //sides.at(i) = (float *);
            //float ** heightMap = (float **)malloc(sizeof(float *)*w);
            double** heightMap = new double*[d];
            for(int j = 0; j < d; j++)
            {
                heightMap[j] = new double[d];
                //heightMap[j] = (float *)malloc(sizeof(float )*h);
                //memset(heightMap[j],255,sizeof(float )*h);
                for(int k = 0; k < d; k++)
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
            i = d-1;
            for (j = 0; j < d; j++) {
                for (k = 0; k < d; k++) {
                    double x = (i/2);
                    double y = ((double)(j - (d/2)));
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += (d);
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(0)[d-k-1][j] += value;

                    //if(k == d-1 && j == 0 || k == 0 && j == 0)
                    //    std::cout << "pos x: {" << point.x << ", " << point.y << ", " << point.z << "} = " << value << " at iteration: " << counter << std::endl;
                    counter++;
                }
            }

            // Negative X:
            i = d-1;
            for (j = 0; j < d; j++) {
                for (k = 0; k < d; k++) {
                    double x = -(i/2);
                    double y = ((double)(j - (d/2)));
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += (d);
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(1)[k][j] += value;

                    //sides.at(1)[k][j] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(1)[k][j] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(1)[k][j] += SimplexNoise(point.z, point.y) * amplitude;
                }
            }
            // Positive Y:
            j = d-1;
            for (i = 0; i < d; i++) {
                for (k = 0; k < d; k++) {

                    double x = (double)(i - (d/2));
                    double y = j/2;
                    double z = (double)(k - (d/2));

                    /*
                    float x = ((float )(i - (w/2)))/(float ) scale;
                    float y = (j/2)/(float ) scale;
                    float z = ((float )(k - (d/2)))/(float ) scale;
                    */

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += (d);
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(2)[i][d-k-1] += value;

                    //sides.at(2)[i][k] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(2)[i][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(2)[i][k] += SimplexNoise(point.x, point.z) * amplitude;
                }
            }

            // Negative Y:
            j = d-1;
            for (i = 0; i < d; i++) {
                for (k = 0; k < d; k++) {

                    double x = ((double)(i - (d/2)));
                    double y = -(j/2);
                    double z = ((double)(k - (d/2)));

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += (d);
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(3)[i][k] += value;

                    //sides.at(3)[i][k] += SimplexNoise3D(x, y, z) * amplitude;
                    //sides.at(3)[i][k] += SimplexNoise3D(point.x, point.y, point.z) * amplitude;
                    //sides.at(3)[i][k] += SimplexNoise(point.x, point.z) * amplitude;
                }
            }

            // Positive Z
            k = d-1;
            for (j = 0; j < d; j++) {
                for (i = 0; i < d; i++) {
                    //float x = (float ) i / (float ) scale;
                    double x = ((double )(i - (d/2)));
                    double y = ((double )(j - (d/2)));
                    double z = (k/2);

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += (d);
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(4)[i][j] += value;

                    counter++;

                    //if(i == w-1 && j == 0 || i == 0 && j == 0)
                    //    std::cout << "pos z: {" << point.x << ", " << point.y << ", " << point.z << "} = " << value << " at iteration: " << counter << std::endl;
                }
            }

            // Negative Z:
            k = d-1;
            for (i = 0; i < d; i++) {
                for (j = 0; j < d; j++) {
                    double x = ((double )(i - (d/2)));
                    double y = ((double )(j - (d/2)));
                    double z = -(k/2);

                    glm::vec3 point = glm::normalize(glm::vec3(x, y, z)) * (float)d;
                    point += d;
                    point /= (double)scale;

                    auto value = SimplexNoise3D(point.x, point.y, point.z) * amplitude;

                    sides.at(5)[d-i-1][j] += value;

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

    std::string OutputImageFloat(int w, int h, float ** heightMap, const char* fileName)
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

    std::string OutputImage(int d, double** heightMap, const char* fileName)
    {
        FILE *f;
        unsigned char *img = NULL;
        int filesize = 54 + d*d*3;  //w is your image width, h is image height, both int

        img = (unsigned char *)malloc(3*d*d);
        memset(img,0,3*d*d);

        int x;
        int y;
        float s;

        double max = -1000000, min = 1000000;

        for(int i=0; i<d; i++)
        {
            for(int j=0; j<d; j++)
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

        for(int i=0; i<d; i++)
        {
            for(int j=0; j<d; j++)
            {
                x=i;
                y=(d-1)-j;

                s = heightMap[i][j] + min;

                s = floor(s);

                if (s > 255) s=255;

                img[(x+y*d)*3+2] = (unsigned char)(s);
                img[(x+y*d)*3+1] = (unsigned char)(s);
                img[(x+y*d)*3+0] = (unsigned char)(s);
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

        bmpinfoheader[ 4] = (unsigned char)(       d    );
        bmpinfoheader[ 5] = (unsigned char)(       d>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       d>>16);
        bmpinfoheader[ 7] = (unsigned char)(       d>>24);
        bmpinfoheader[ 8] = (unsigned char)(       d    );
        bmpinfoheader[ 9] = (unsigned char)(       d>> 8);
        bmpinfoheader[10] = (unsigned char)(       d>>16);
        bmpinfoheader[11] = (unsigned char)(       d>>24);

        std::string outputFile = folder;
        outputFile.append(fileName);

        f = fopen(outputFile.c_str(),"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<d; i++)
        {
            fwrite(img+(d*(d-i-1)*3),3,d,f);
            fwrite(bmppad,1,(4-(d*3)%4)%4,f);
        }

        free(img);
        fclose(f);
        return outputFile;
    }

    std::string OutputImageBytes(int d, GLubyte* heightMap, const char* fileName)
    {
        FILE *f;
        unsigned char *img = NULL;
        int filesize = 54 + d*d*3;  //w is your image width, h is image height, both int

        img = (unsigned char *)malloc(3*d*d);
        memset(img,0,3*d*d);

        int x;
        int y;
        float s;

        for(int i=0; i<d; i++)
        {
            for(int j=0; j<d; j++)
            {
                x=i;
                y=(d-1)-j;

                s = heightMap[x+y*d];

                if (s > 255) s=255;

                img[(x+y*d)*3+2] = (unsigned char)(s);
                img[(x+y*d)*3+1] = (unsigned char)(s);
                img[(x+y*d)*3+0] = (unsigned char)(s);
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

        bmpinfoheader[ 4] = (unsigned char)(       d    );
        bmpinfoheader[ 5] = (unsigned char)(       d>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       d>>16);
        bmpinfoheader[ 7] = (unsigned char)(       d>>24);
        bmpinfoheader[ 8] = (unsigned char)(       d    );
        bmpinfoheader[ 9] = (unsigned char)(       d>> 8);
        bmpinfoheader[10] = (unsigned char)(       d>>16);
        bmpinfoheader[11] = (unsigned char)(       d>>24);

        std::string outputFile = folder;
        outputFile.append(fileName);

        f = fopen(outputFile.c_str(),"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);
        for(int i=0; i<d; i++)
        {
            fwrite(img+(d*(d-i-1)*3),3,d,f);
            fwrite(bmppad,1,(4-(d*3)%4)%4,f);
        }

        free(img);
        fclose(f);
        return outputFile;
    }

    std::vector<std::string> OutputCubeMapImage(int d, std::vector<double**> sides, const char* fileName)
    {
        std::vector<std::string> fileNames;

        std::string outputFileName= "cube/PosX_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(0), outputFileName.c_str()));

        outputFileName= "cube/NegX_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(1), outputFileName.c_str()));



        outputFileName= "cube/PosY_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(2), outputFileName.c_str()));
        outputFileName= "cube/NegY_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(3), outputFileName.c_str()));


        outputFileName= "cube/PosZ_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(4), outputFileName.c_str()));
        outputFileName= "cube/NegZ_";
        outputFileName.append(fileName);
        fileNames.push_back(OutputImage(d, sides.at(5), outputFileName.c_str()));

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
        float s;

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
