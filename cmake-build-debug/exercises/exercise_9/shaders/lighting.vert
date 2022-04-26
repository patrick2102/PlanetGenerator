#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 projPosition;

void main()
{
   // Pass the projected position to fragment shader
   projPosition = viewProjection * model * vec4(vertex, 1.0);

   gl_Position = projPosition;
}