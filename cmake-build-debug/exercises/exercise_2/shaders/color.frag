#version 330 core
// FRAGMENT SHADER

// fragColor is the output color that OpenGL will try to draw in the screen, if it's not occluded.
out vec4 fragColor;
// Create a 'uniform vec3' to receive the cone color from your application.
uniform vec3 uCol;

void main()
{
    // set the fragColor using the 'uniform vec3' color that you set for the current object
    // notice that fragColor is a vec4, the last value is used to set opacity and should be set to 1
    //fragColor = vec4(cCol.x, cCol.y, 1.0, 1.0); // CODE HERE
    fragColor = vec4(uCol.x, uCol.y, uCol.z, 1.0);
}