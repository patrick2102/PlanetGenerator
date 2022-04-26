#version 330 core
// FRAGMENT SHADER

// TODO exercise 2.5
out vec4 fragColor;
// Create an 'in float' variable to receive the depth value from the vertex shader,
// the variable must have the same name as the 'out variable' in the vertex shader.
in float zCoords;

// Create a 'uniform vec3' to receive the cone color from your application.
uniform vec3 uCol;

void main()
{
    // Modulate the color of the fragColor using the z-coordinate of the fragment.
    // Make sure that the z-coordinate is in the [0, 1] range (if it is not, place it in that range),
    // you can use non-linear transformations of the z-coordinate, such as the 'pow' or 'sqrt' functions,
    // to make the colors brighter close to the center of the cone.
    //fragColor = vec4(zCoords - sqrt(uCol.x), zCoords - sqrt(uCol.y), zCoords - sqrt(uCol.z), 1.0);
    //fragColor = vec4(uCol.x - zCoords, uCol.y - zCoords, uCol.z - zCoords, 1.0);
    float dist = 1-pow(zCoords);
    fragColor = vec4(uCol.x - dist, uCol.y - dist, uCol.z - dist, 1.0);
}