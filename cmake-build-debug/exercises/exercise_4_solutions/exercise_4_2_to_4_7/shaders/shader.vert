#version 330 core
layout (location = 0) in vec2 pos;   // the position variable has attribute position 0
// TODO 4.2 add velocity and timeOfBirth as vertex attributes
layout (location = 1) in vec2 velocity;
layout (location = 2) in float timeOfBirth;

// TODO 4.3 create and use a float uniform for currentTime
uniform float currentTime;

// TODO 4.6 create out variable to send the age of the particle to the fragment shader
out float elapsedTimeFrag;

const float maxAge = 10.0;

void main()
{
    // TODO 4.3 use the currentTime to control the particle in different stages of its lifetime
    vec2 finalPos = pos;
    float elapsedTime = currentTime - timeOfBirth;
    if (timeOfBirth == 0 || elapsedTime > maxAge){
        finalPos = vec2(-2.0f, -2.0f);
    } else {
        finalPos += velocity * elapsedTime;
    }
	
    // TODO 4.6 send the age of the particle to the fragment shader using the out variable you have created

    elapsedTimeFrag = elapsedTime;

    gl_Position = vec4(finalPos, 0.0, 1.0);
    gl_PointSize = (elapsedTime * 2.0) + 1.0;
}