#version 330 core

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightRadius;

// material properties
uniform vec3 reflectionColor;
uniform float ambientReflectance;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

// material textures
// TODO 7.3 : Add a sampler2D uniform for the albedo texture. Name must be 'textureAlbedo' 
uniform sampler2D textureAlbedo;


// TODO 7.3 : Add an 'in' variable for texture coordinates
in vec2 textureCoordinates;
// TODO 7.4 : Add an 'in' variable for view normal
in vec3 viewNormal;


// output colors of this fragment
out vec3 AlbedoGBuffer;
out vec2 NormalGBuffer;
out vec4 OthersGBuffer;

void main()
{
   // TODO 7.3 : Sample your albedo texture using the texture coordinates from vertex shader
   vec3 albedo = texture(textureAlbedo, textureCoordinates).xyz;

   // TODO 7.3 : Multiply albedo by the reflection color
   albedo *= reflectionColor;

   // Output albedo to GBuffer
   AlbedoGBuffer = albedo;

   // TODO 7.4 : Get the normal from the vertex shader and normalize it
   vec3 normal = normalize(viewNormal);

   // Output normal to GBuffer (only 2 components)
   NormalGBuffer = normal.xy;

   // TODO 7.5 : Pack the remaining data - Remember values must be normalized in range [0, 1]
   vec4 others = vec4(ambientReflectance, diffuseReflectance, specularReflectance, specularExponent / 100.0f);

   // Output others to GBuffer
   OthersGBuffer = others;
}
