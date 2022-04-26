#version 330 core
// VERTEX SHADER

// TODO exercise 2.3
// Receives position in 'vec3', the position variable has attribute 'location = 0'
layout (location = 0) in vec3 pos;
// You have to declare an 'out float' to send the z-coordinate of the position
// to the fragment shader (exercise 2.4 and 2.5)
out float depth;
// You have to set an 'uniform vec2' to receive the position offset of the object
uniform vec2 uPosition;

void main()
{
    // TODO exercise 2.3
    // Set the vertex->fragment shader 'out' variable
    depth = pos.z;
    // Set the 'gl_Position' built-in variable using a 'vec4(vec3 position you compute, 1.0)',
    // Remeber to use the 'uniform vec2' to move the vertex before you set 'gl_Position'.
    gl_Position = vec4(pos.x + uPosition.x, pos.y + uPosition.y, pos.z, 1.0);
}