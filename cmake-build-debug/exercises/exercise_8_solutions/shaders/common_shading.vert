#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;
layout (location = 3) in vec3 tangent;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined
uniform mat4 lightSpaceMatrix;   // transforms from world space to light space

out vec4 worldPos;
out vec3 worldNormal;
out vec3 worldTangent;
out vec2 textureCoordinates;

// TODO 8.1 : Add an 'out' variable for vertex position in light space
out vec4 lightPos;

void main() {
   // vertex in world space (for lighting computation)
   worldPos = model * vec4(vertex, 1.0);
   // normal in world space (for lighting computation)
   worldNormal = (model * vec4(normal, 0.0)).xyz;
   // tangent in world space (for lighting computation)
   worldTangent = (model * vec4(tangent, 0.0)).xyz;

   // TODO 8.1 : obtain and pass to fragment shader the position in light space, converting from world space
   lightPos = lightSpaceMatrix * worldPos;

   textureCoordinates = textCoord;

   // final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * worldPos;
}
