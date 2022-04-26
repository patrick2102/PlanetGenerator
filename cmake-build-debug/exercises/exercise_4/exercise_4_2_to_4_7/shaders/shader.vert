#version 330 core
layout (location = 0) in vec2 pos;   // the position variable has attribute position 0
layout (location = 1) in vec2 vel; // Velocity vector
layout (location = 2) in float tob; // Time of birth

out float age;

uniform float currentTime;


void main()
{
    float gravity = 0.025f;
    if(tob == 0.0)
    {
        gl_Position = vec4(1.1, 1.1, 0.0, 1.0);
    }
    else
    {
        age = currentTime - tob;
        if(age > 10.0)
        {
            gl_Position = vec4(1.1, 1.1, 0.0, 1.0);
        } else {
            vec2 newPos = pos + (age*vel);
            newPos.y = newPos.y - (age * age * gravity);
            gl_Position = vec4(newPos, 0.0, 1.0);
            gl_PointSize = age*2;
        }
    }
}