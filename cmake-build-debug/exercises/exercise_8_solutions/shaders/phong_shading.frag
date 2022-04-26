#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec4 ambientLightColor;
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
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_ambient1;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;
uniform sampler2D shadowMap;

// 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec4 worldPos;
in vec3 worldNormal;
in vec3 worldTangent;
in vec2 textureCoordinates;

// TODO 8.1 : Add an 'in' variable for vertex position in light space
in vec4 lightPos;


vec3 GetNormalMap()
{
   //NEW! Normal map

   // Sample normal map
   vec3 normalMap = texture(texture_normal1, textureCoordinates).rgb;
   // Unpack from range [0, 1] to [-1 , 1]
   normalMap = normalMap * 2.0 - 1.0;

   // This time we are storing Z component in the texture, no need to compute it. Instead we normalize just in case
   normalMap = normalize(normalMap);

   // Create tangent space matrix
   vec3 N = normalize(worldNormal);
   vec3 B = normalize(cross(worldTangent, N)); // Orthogonal to both N and T
   vec3 T = cross(N, B); // Orthogonal to both N and B. Since N and B are normalized and orthogonal, T is already normalized
   mat3 TBN = mat3(T, B, N);

   // Transform normal map from tangent space to world space
   return TBN * normalMap;
}

vec3 GetAmbientLighting(vec3 albedo)
{
   vec3 ambient = ambientLightColor.rgb * ambientReflectance * albedo;

   // NEW! Ambient occlusion (try disabling it to see how it affects the visual result)
   float ambientOcclusion = texture(texture_ambient1, textureCoordinates).r;
   ambient *= ambientOcclusion;

   return ambient;
}

vec3 GetEnvironmentLighting(vec3 N, vec3 V)
{
   //NEW! Environment reflection

   // Compute reflected light vector (R)
   vec3 R = reflect(-V, N);

   // Sample cubemap
   vec3 reflection = texture(skybox, R).rgb;

   // We packed the amount of reflection in ambientLightColor.a
   // Only apply reflection (and ambient) during the first light pass
   reflection *= ambientLightColor.a; 

   // Environment reflection is specular and it gets affected by specularReflectance
   reflection *= specularReflectance;

   // NEW! We use the metalness texture to select when materials reflect more
   // This is not physically accurate, but for now it is ok to get some highlights
   float metalness = texture(texture_specular1, textureCoordinates).r;
   reflection *= mix(0.2f, 0.8f, metalness);

   return reflection;
}

vec3 GetLambertianDiffuseLighting(vec3 N, vec3 L, vec3 albedo)
{
   vec3 diffuse = diffuseReflectance * albedo;

   return diffuse;
}

vec3 GetBlinnPhongSpecularLighting(vec3 N, vec3 L, vec3 V)
{
   vec3 H = normalize(L + V);

   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   vec3 specular = vec3(specularReflectance) * specModulation;

   return specular;
}

float GetAttenuation(vec4 P)
{
   float distToLight = distance(lightPosition, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);

   float falloff = smoothstep(lightRadius, lightRadius*0.5f, distToLight);

   return attenuation * falloff;
}

float GetShadow()
{
   // TODO 8.1 : Transform the position in light space to shadow map space: from range (-1, 1) to range (0, 1)
   vec3 shadowMapSpacePos = (lightPos.xyz / lightPos.w) * 0.5 + 0.5;

   // TODO 8.1 : Sample the shadow map texture using the XY components of the light in shadow map space
   float depth = texture(shadowMap, shadowMapSpacePos.xy).r;

   // TODO 8.1 : Compare the depth value obtained with the Z component of the light in shadow map space. Return 0 if depth is smaller or equal, 1 otherwise
   return depth + 0.01f <= clamp(shadowMapSpacePos.z, -1, 1) ? 0.0 : 1.0;
}

void main()
{
   vec4 P = worldPos;

   vec3 N = GetNormalMap();

   vec3 albedo = texture(texture_diffuse1, textureCoordinates).xyz;
   albedo *= reflectionColor;

   bool positional = lightRadius > 0;

   vec3 L = normalize(lightPosition - (positional ? P.xyz : vec3(0.0f)));
   vec3 V = normalize(camPosition - P.xyz);

   vec3 ambient = GetAmbientLighting(albedo);
   vec3 environment = GetEnvironmentLighting(N, V);

   vec3 diffuse = GetLambertianDiffuseLighting(N, L, albedo);
   vec3 specular = GetBlinnPhongSpecularLighting(N, L, V);

   // This time we get the lightColor outside the diffuse and specular terms (we are multiplying later)
   vec3 lightRadiance = lightColor;

   // Modulate the radiance with the angle of incidence
   lightRadiance *= max(dot(N, L), 0.0);

   // Modulate lightRadiance by distance attenuation (only for positional lights)
   float attenuation = positional ? GetAttenuation(P) : 1.0f;
   lightRadiance *= attenuation;

   // Modulate lightRadiance by shadow (only for directional light)
   float shadow = positional ? 1.0f : GetShadow();
   lightRadiance *= shadow;

   vec3 indirectLight = ambient + environment;

   vec3 directLight = diffuse + specular;
   directLight *= lightRadiance;

   // lighting = indirect lighting (ambient + environment) + direct lighting (diffuse + specular)
   vec3 lighting = indirectLight + directLight;

   FragColor = vec4(lighting, 1.0f);
}
