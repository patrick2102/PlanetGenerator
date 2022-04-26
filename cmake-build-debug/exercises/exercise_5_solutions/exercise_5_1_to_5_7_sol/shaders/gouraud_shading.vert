#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord; // here for completness, but we are not using it just yet

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined
uniform vec3 camPosition; // so we can compute the view vector

// send shaded color to the fragment shader
out vec4 shadedColor;

// TODO exercise 5 setup the uniform variables needed for lighting
// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 light1Position;
uniform vec3 light1Color;

// material properties
uniform vec3 reflectionColor;
uniform float ambientReflectance;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

void main() {
   // vertex in world space (for light computation)
   vec4 P = model * vec4(vertex, 1.0);
   // normal in world space (for light computation)
   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   // final vertex transform (for opengl rendering, not for lighting)
   gl_Position = viewProjection * P;

   // TODO exercises 5.1, 5.2 and 5.3 - Gouraud shading (i.e. Phong reflection model computed in the vertex shader)

   // TODO 5.1 ambient
   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;

   // TODO 5.2 diffuse
   vec3 L = normalize(light1Position - P.xyz);
   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = light1Color * diffuseReflectance * diffuseModulation * reflectionColor;

   // TODO 5.3 specular
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   // Alternative version: Same result, with different specularExponent values
   //vec3 R = 2 * dot(L, N) * N - L; // the same as reflect(-L, normal)
   //float specModulation = pow(max(dot(R, V), 0.0), specularExponent);
   vec3 specular = light1Color * specularReflectance * specModulation;

   // notice that we did not use the material color (reflectionColor) in the specular, that is because most materials
   // do not affect the specular highlight color, with exception of metals (you can play with that)

   // TODO exercise 5.5 - attenuation - light 1
   float distToLight = distance(light1Position, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);

   // TODO set the output color to the shaded color that you have computed
   shadedColor = vec4(ambient + (diffuse + specular) * attenuation, 1.0);
}