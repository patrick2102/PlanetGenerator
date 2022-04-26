#version 330 core
layout (location = 0) in vec3 vertex;

out vec2 textureCoordinates;

void main()
{
   textureCoordinates = vertex.xy * 0.5f + 0.5f;

   gl_Position = vec4(vertex, 1.0f);
}