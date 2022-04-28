//
// Created by Patrick on 28/04/2022.
//
#include "Sphere.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_PLANET_H
#define ITU_GRAPHICS_PROGRAMMING_PLANET_H



class Planet
{
public:
    Sphere sphere;
    glm::vec4 baseColor = glm::vec4(0.5f, 0.25f, 0.10f, 1.0f);

    Planet(Sphere sphere) : sphere(sphere) {

    }
    void Draw()
    {
        //sphere.Draw();
    }

};
#endif //ITU_GRAPHICS_PROGRAMMING_PLANET_H