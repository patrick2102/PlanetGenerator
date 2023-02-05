#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;

out vec4 worldPos;
out vec3 worldNormal;
out float height;
out vec3 outColor;
out vec3 localPos;
out vec3 localNormal;

void main() {

   vec3 localPos = vertex;

   float displace = texture(surfaceTexture, localPos).r - 0.025;

   displace = clamp(displace, -1.0f, 1.0f);

   localPos = localPos + (normalize(localPos) * displace);

   vec4 P = model * vec4(localPos, 1.0);

   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   localPos = vertex;
   localNormal = normal;
   worldPos = P;
   worldNormal = N;

   outColor = color;
   height = displace;
   gl_Position = viewProjection * P;
}