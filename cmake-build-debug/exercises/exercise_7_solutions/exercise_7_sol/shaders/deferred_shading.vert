#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 view; // represents the view matrix
uniform mat4 viewProjection;  // represents the view and projection matrices combined

// TODO 7.3 : Add an 'out' variable for texture coordinates
out vec2 textureCoordinates;

// TODO 7.4 : Add an 'out' variable for view normal
out vec3 viewNormal;

void main() {

   // TODO 7.3 : Read the texture coordinates from the attribute and pass it to the fragment shader
   textureCoordinates = textCoord;

   // TODO 7.4 : Compute the normal in VIEW space and pass it to the fragment shader
   viewNormal = (view * model * vec4(normal, 0.0f)).xyz;

   // Final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * model * vec4(vertex, 1.0f);
}