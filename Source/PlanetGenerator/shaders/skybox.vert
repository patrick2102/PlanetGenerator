#version 330 core
layout (location = 0) in vec3 vertex;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
   // The texture coordinate is now a direction.
   // Because the cube representing the skybox is centered around the (0,0,0) position
   // we can simply use the vertex position to describe a direction
   TexCoords = vertex;
   // We ignore the translation in our view matrix since we don't want to move the box
   // repersenting the skybox relative to the camera.
   // To do that we extract the top left part of the matrix with mat3, and multiply by vertex without
   // the homogeneous coordinate
   vec4 pos = projection * vec4(mat3(view) * vertex, 1.0);
   // Notice this interesting manipulation, we use w as the z coordinate, why do you think this is the case?
   gl_Position = pos.xyww;
}  

