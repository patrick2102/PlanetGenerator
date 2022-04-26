#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 worldPos;
out vec3 worldNormal;

// TODO 7.3 : Add an 'out' variable for texture coordinates
out vec2 textureCoordinates;

void main() {
   // vertex in world space (for lighting computation)
   vec4 P = model * vec4(vertex, 1.0);
   // normal in world space (for lighting computation)
   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   // pass the positions in world space to the fragment shader
   worldPos = P;
   worldNormal = N;

   // TODO 7.3 : Read the texture coordinates from the attribute and pass it to fragment
   textureCoordinates = textCoord;

   // final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * P;
}