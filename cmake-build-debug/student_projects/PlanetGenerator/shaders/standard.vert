#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 vtxColor;

void main()
{
   gl_Position = viewProjection * vec4(pos, 1.0);
   vtxColor = color;
}