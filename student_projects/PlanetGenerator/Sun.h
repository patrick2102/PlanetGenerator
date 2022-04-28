//
// Created by Patrick on 28/04/2022.
//

#include "Sphere.h"
#include "Misc.h"

#ifndef ITU_GRAPHICS_PROGRAMMING_SUN_H
#define ITU_GRAPHICS_PROGRAMMING_SUN_H



class Sun
{
public:
    glm::vec3 position;
    Sphere sphere;
    //Light light;
    glm::vec4 baseColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);

    Sun(Sphere sphere) : sphere(sphere) {

    }

    void Draw()
    {
        sphere.Draw();
    }

};
#endif //ITU_GRAPHICS_PROGRAMMING_SUN_H