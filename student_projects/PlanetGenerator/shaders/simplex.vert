#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;

out vec4 worldPos;
out vec3 worldNormal;
out vec3 TexCoords;

void main() {


   /*
   // vertex in world space (for lighting computation)
   vec4 P = model * vec4(vertex, 1.0);
   // normal in world space (for lighting computation)
   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   TexCoords = vertex;

   worldPos = P;
   worldNormal = N;

   */
   TexCoords = vertex;

   vec3 localPos = vertex;

   vec4 displace = texture(displacementMap, TexCoords);

   localPos = localPos + (normal * displace.r);

   vec4 P = model * vec4(localPos, 1.0);


   gl_Position = viewProjection * P;
}