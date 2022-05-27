#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 vtxColor;

void main() {

   gl_Position = viewProjection * vec4(position, 1.0);
   vtxColor = vec4(color, 1.0f);
}