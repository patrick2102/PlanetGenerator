#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

uniform int scale;
uniform float amplitude;
uniform float persistence;
uniform float lacunarity;
uniform int diameter;
uniform int iterations;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 worldPos;
out vec3 worldNormal;
out vec3 localPos;

void main() {
   //atmosColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ) );
   vec3 lpos = vertex;
   lpos = vertex * 1.4; // * outer_radius;
   vec4 P = model * vec4(lpos, 1.0);

   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   //localNormal = normal;
   localPos = lpos;
   worldPos = P;
   worldNormal = N;
   //atmosColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ) );
   //atmosColor = atmosphereScatter2();

   gl_Position = viewProjection * P;
}