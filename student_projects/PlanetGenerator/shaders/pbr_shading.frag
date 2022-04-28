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
uniform float roughness;
uniform float metalness;

// legacy uniforms, not needed for PBR
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

in vec4 lightPos;

// Constant Pi
const float PI = 3.14159265359;


// Schlick approximation of the Fresnel term
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
   return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float a)
{
   float a2 = a*a;
   float NdotH = max(dot(N, H), 0.0);
   float NdotH2 = NdotH*NdotH;

   float num = a2;
   float denom = (NdotH2 * (a2 - 1.0) + 1.0);
   denom = PI * denom * denom;

   return num / denom;
}

float GeometrySchlickGGX(float cosAngle, float a)
{
   float a2 = a*a;

   float num = 2 * cosAngle;
   float denom = cosAngle + sqrt(a2 + (1 - a2)*cosAngle*cosAngle);

   return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float a)
{
   float NdotV = max(dot(N, V), 0.0);
   float NdotL = max(dot(N, L), 0.0);
   float ggx2  = GeometrySchlickGGX(NdotV, roughness);
   float ggx1  = GeometrySchlickGGX(NdotL, roughness);

   return ggx1 * ggx2;
}

vec3 GetCookTorranceSpecularLighting(vec3 N, vec3 L, vec3 V)
{
   vec3 H = normalize(L + V);

   // Remap alpha parameter to roughness^2
   float a = roughness * roughness;

   float D = DistributionGGX(N, H, a);
   float G = GeometrySmith(N, V, L, a);

   float cosI = max(dot(N, L), 0.0);
   float cosO = max(dot(N, V), 0.0);

   // Important! Notice that Fresnel term (F) is not here because we apply it later when mixing with diffuse
   float specular = (D * G) / (4.0f * cosO * cosI + 0.0001f);

   return vec3(specular);
}

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

vec3 GetAmbientLighting(vec3 albedo, vec3 normal)
{
   //vec3 ambient = ambientLightColor.rgb * albedo;

   vec3 ambient = textureLod(skybox, normal, 5.0f).rgb;

   ambient *= albedo / PI;

   // Only apply ambient during the first light pass
   ambient *= ambientLightColor.a; 

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
   // HACK: We sample a different mipmap depending on the roughness. Rougher surface will have blurry reflection
   vec3 reflection = textureLod(skybox, R, roughness * 5.0f).rgb;

   // We packed the amount of reflection in ambientLightColor.a
   // Only apply reflection (and ambient) during the first light pass
   reflection *= ambientLightColor.a; 

   return reflection;
}

vec3 GetLambertianDiffuseLighting(vec3 N, vec3 L, vec3 albedo)
{
   vec3 diffuse = diffuseReflectance * albedo;

   diffuse /= PI;

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
   vec3 shadowMapSpacePos = (lightPos.xyz / lightPos.w) * 0.5 + 0.5;

   float depth = texture(shadowMap, shadowMapSpacePos.xy).r;

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

   vec3 ambient = GetAmbientLighting(albedo, N);
   vec3 environment = GetEnvironmentLighting(N, V);

   vec3 diffuse = GetLambertianDiffuseLighting(N, L, albedo);

   vec3 specular = GetCookTorranceSpecularLighting(N, L, V);

   // This time we get the lightColor outside the diffuse and specular terms (we are multiplying later)
   vec3 lightRadiance = lightColor;

   // Modulate lightRadiance by distance attenuation (only for positional lights)
   float attenuation = positional ? GetAttenuation(P) : 1.0f;
   lightRadiance *= attenuation;

   // Modulate lightRadiance by shadow (only for directional light)
   float shadow = positional ? 1.0f : GetShadow();
   lightRadiance *= shadow;

   // Modulate the radiance with the angle of incidence
   lightRadiance *= max(dot(N, L), 0.0);

   // We use a fixed value of 0.04f for F0. The range in dielectrics is usually in the range (0.02, 0.05)
   vec3 F0 = vec3(0.04f);

   F0 = mix(F0, albedo, metalness);

   diffuse = mix(diffuse, vec3(0), metalness);
   ambient = mix(ambient, vec3(0), metalness);

   vec3 FAmbient = FresnelSchlick(F0, max(dot(N, V), 0.0));

   vec3 indirectLight = mix(ambient, environment, FAmbient);

   vec3 H = normalize(L + V);
   vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));

   vec3 directLight = mix(diffuse, specular, F);
   //vec3 directLight = diffuse + specular;
   directLight *= lightRadiance;

   // lighting = indirect lighting (ambient + environment) + direct lighting (diffuse + specular)
   vec3 lighting = indirectLight + directLight;

   FragColor = vec4(lighting, 1.0f);
}
