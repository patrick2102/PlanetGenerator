#version 330 core
//layout (location = 0) in vec3 vertex;
layout (location = 0) in vec3 vertex;

//out vec3 pos;
out vec3 pos;

void main()
{
   //gl_Position = vec4(vertex, 1.0);
   gl_Position = vec4(vertex, 1.0);
   pos = vertex;
}