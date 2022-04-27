#version 330 core
out vec4 FragColor;
in  vec4 vtxColor;

void main()
{
   FragColor = vtxColor;
}