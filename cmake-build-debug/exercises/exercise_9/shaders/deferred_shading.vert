#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;
layout (location = 3) in vec3 tangent;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined
uniform vec4 texCoordTransform; // scale and offset for texture coordinates

// variables to fragment shader
out vec2 textureCoordinates;
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 worldTangent;

void main() {

   // Read the texture coordinates from the attribute and pass it to the fragment shader
   textureCoordinates = textCoord * texCoordTransform.xy + texCoordTransform.zw;

   // Compute the position in world space and pass it to the fragment shader
   worldPosition =  (model * vec4(vertex, 1.0f)).xyz;

   // Compute the normal in world space and pass it to the fragment shader
   worldNormal = (model * vec4(normal, 0.0f)).xyz;

   // Compute the tangent in world space and pass it to the fragment shader
   worldTangent = (model * vec4(tangent, 0.0f)).xyz;

   // Final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * vec4(worldPosition, 1);
}