#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec4 color;

//uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

//out vec4 worldPos;
//out vec3 worldNormal;
out vec4 vtxColor;

void main() {
   /*
   // vertex in world space (for lighting computation)
   vec4 P = model * vec4(vertex, 1.0);
   // normal in world space (for lighting computation)
   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   worldPos = P;
   worldNormal = N;

   // final vertex position (for opengl rendering, not for lighting)
   //gl_Position = viewProjection * P;
   //gl_Position = viewProjection * ve4vertex;
   */
   gl_Position = viewProjection * vec4(vertex, 1.0);
   vtxColor = color;
}