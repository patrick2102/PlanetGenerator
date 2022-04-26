#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

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

// 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec4 worldPos;
in vec3 worldNormal;

// TODO 7.3 : Add an 'in' variable for texture coordinates
in vec2 textureCoordinates;

void main()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);

   // TODO 7.3 : Sample your albedo texture using the texture coordinates from vertex shader
   vec3 albedo = texture(textureAlbedo, textureCoordinates).xyz;
   albedo *= reflectionColor;

   // phong shading (i.e. Phong reflection model computed in the fragment shader)
   vec3 ambient = ambientLightColor * ambientReflectance * albedo;

   vec3 L = normalize(lightPosition - P.xyz);
   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = lightColor * diffuseReflectance * diffuseModulation * albedo;

   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   vec3 specular = lightColor * specularReflectance * specModulation;

   float distToLight = distance(lightPosition, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);

   // TODO 7.1 : Compute the falloff using lightRadius and smoothstep function
   float falloff = smoothstep(lightRadius, lightRadius*0.5f, distToLight);

   // TODO 7.1 : Multiply the attenuation by the falloff we just computed
   attenuation *= falloff;

   FragColor = vec4(ambient + (diffuse + specular) * attenuation, 1.0);
}
