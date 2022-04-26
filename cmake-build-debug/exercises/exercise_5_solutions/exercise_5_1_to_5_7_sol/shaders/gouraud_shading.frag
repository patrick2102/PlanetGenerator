#version 330 core

in vec4 shadedColor;
out vec4 FragColor;

void main()
{
   // there is no need to modify this file for Gouraud shading
   // we pass through the interpolated color since lighting was already computed in the vertex shader
   FragColor = shadedColor;
}