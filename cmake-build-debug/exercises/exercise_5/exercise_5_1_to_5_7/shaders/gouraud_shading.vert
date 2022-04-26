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
/* Ambience */
uniform float ambientLightIntensity;
/* Light 1 */
uniform float light1Intensity;
uniform vec3 light1Position;
uniform vec3 light1Color;


// material properties
/* Ambience */
uniform float ambientReflectance;
uniform vec3 reflectionColor; // The color of the object
/* Diffuse */
uniform float diffuseReflectance;
/* Specular */
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
   vec3 reflection = ambientLightIntensity * ambientReflectance * reflectionColor;

   // TODO 5.2 diffuse
   vec3 L = normalize(light1Position-P.xyz);
   reflection += (light1Intensity * diffuseReflectance * (dot(N, L)) * reflectionColor);

   // TODO 5.3 specular
   vec3 V = normalize(camPosition-P.xyz);
   vec3 H = normalize(L + V);
   reflection += (light1Intensity * specularReflectance * pow((dot(N, H)), specularExponent));

   // TODO exercise 5.5 - attenuation - light 1


   // TODO set the output color to the shaded color that you have computed
   //shadedColor = vec4(.8, .8, .8, 1.0);
   shadedColor = vec4(reflection, 1.0);

}