#version 330 core

uniform mat4 view; // represents the view matrix
uniform vec3 cameraPosition;

// material properties
uniform vec3 reflectionColor;
uniform float roughness;
uniform float metalness;

// material textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_ambient1;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;

// variables from vertex shader
in vec2 textureCoordinates;
in vec3 worldPosition;
in vec3 worldNormal;
in vec3 worldTangent;

// output colors of this fragment
out vec3 AlbedoGBuffer;
out vec2 NormalGBuffer;
out vec4 OthersGBuffer;
out vec4 AccumBuffer;


// Constant Pi
const float PI = 3.14159265359;


vec3 GetNormalMap(vec3 normalMap)
{
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

vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
   return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 GetAmbientLighting(vec3 albedo, vec3 N)
{
   vec3 ambient = textureLod(skybox, N, 4.0f).rgb;

   ambient *= albedo / PI;

   float ambientOcclusion = texture(texture_ambient1, textureCoordinates).r;
   ambient *= ambientOcclusion;

   return ambient;
}

vec3 GetEnvironmentLighting(vec3 N, vec3 V, float roughness)
{
   // Compute reflected light vector (R)
   vec3 R = reflect(-V, N);

   // Sample cubemap
   // HACK: We sample a different mipmap depending on the roughness. Rougher surface will have blurry reflection
   vec3 reflection = textureLod(skybox, R, roughness * roughness * 5.0f).rgb;

   return reflection;
}

void main()
{
   vec3 albedoMap = texture(texture_diffuse1, textureCoordinates).xyz;
   vec3 albedo = albedoMap * reflectionColor;

   AlbedoGBuffer = albedo;

   vec3 normalMap = texture(texture_normal1, textureCoordinates).rgb;
   vec3 N = GetNormalMap(normalMap);
   NormalGBuffer = (view * vec4(N, 0)).xy;

   vec4 specular = texture(texture_specular1, textureCoordinates); // not used, not good sample textures
   OthersGBuffer = vec4(roughness, metalness, 0.0f, 0.0f);

   vec3 V = normalize(cameraPosition - worldPosition);

   vec3 ambient = GetAmbientLighting(albedo, N);
   ambient = mix(ambient, vec3(0), metalness);

   vec3 environment = GetEnvironmentLighting(N, V, roughness);

   vec3 F0 = vec3(0.04f);
   F0 = mix(F0, albedo, metalness);
   vec3 F = FresnelSchlick(F0, max(dot(N, V), 0.0));

   vec3 indirectLight = mix(ambient, environment, F);

   // HACK: we are hardcoding this to get red colors as emissive for the exercise, please remove if you want to reuse this code
   vec3 emissive = max(0, albedoMap.r - albedoMap.g - albedoMap.b) * 20.0f * albedo;

   AccumBuffer = vec4(indirectLight + emissive, 1.0f);
}
