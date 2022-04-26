#version 330 core
// VERTEX SHADER

// Receives position in 'vec3', the position variable has attribute 'location = 0'
layout (location = 0) in vec3 aPos;

// You have to declare an 'out float' to send the z-coordinate of the position to the fragment shader (exercise 2.4 and 2.5)
out float zCoords;

// You have to set an 'uniform vec2' to receive the position offset of the object
uniform vec2 uPos;

void main()
{
    // Set the vertex->fragment shader 'out' variable
    zCoords = aPos.z;
    // Set the 'gl_Position' built-in variable using a 'vec4(vec3 position you compute, 1.0)',
    // Remember to use the 'uniform vec2' to move the vertex before you set 'gl_Position'.
    gl_Position = vec4(aPos.x + uPos.x, aPos.y + uPos.y, aPos.z, 1.0);
}