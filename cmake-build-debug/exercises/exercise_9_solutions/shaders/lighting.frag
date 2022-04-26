#version 330 core

// transform matrices
uniform mat4 invProjection; // transform from clip space to view space

// light uniform variables
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightRadius;
uniform mat4 lightSpaceMatrix;   // transforms from view space to light space

// g-buffers
uniform sampler2D AlbedoGBuffer;
uniform sampler2D NormalGBuffer;
uniform sampler2D OthersGBuffer;
uniform sampler2D DepthBuffer;
uniform sampler2D ShadowMap;

in vec4 projPosition;

out vec4 FragColor; // the output color of this fragment


// Constant Pi
const float PI = 3.14159265359;


vec3 ReconstructPosition(vec4 projPosition, float depth)
{
   // Transform depth to range [-1, 1]
   depth = depth * 2 - 1;

   // Reconstruct clipPosition from projPosition(X,Y) and depth (Z)
   vec3 clipPosition = vec3(projPosition.xy / projPosition.w, depth);

   // Multiply clipPosition by inverse projection matrix to change to view space
   vec4 P = invProjection * vec4(clipPosition, 1.0f);

   // Divide by P.w after projecting
   P = P / P.w;

   return P.xyz;
}

vec3 ReconstructNormal(vec2 normalMap)
{
   vec3 normal = vec3(normalMap, 0);
   // Reconstruct Z component of the normal, knowing that the normal length is 1  (X*X + Y*Y + Z*Z = 1)
   normal.z = sqrt(1 - normal.x*normal.x - normal.y*normal.y);
   return normal;
}


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
   float ggx2  = GeometrySchlickGGX(NdotV, a);
   float ggx1  = GeometrySchlickGGX(NdotL, a);

   return ggx1 * ggx2;
}

vec3 GetCookTorranceSpecularLighting(vec3 N, vec3 L, vec3 V, float roughness)
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

vec3 GetLambertianDiffuseLighting(vec3 albedo)
{
   // Diffuse scattered in all directions
   return albedo / PI;
}

float GetAttenuation(vec3 P)
{
   float distToLight = distance(lightPosition, P);
   float attenuation = 1.0f / (distToLight * distToLight);

   float falloff = smoothstep(lightRadius, lightRadius*0.5f, distToLight);

   return attenuation * falloff;
}

float GetShadow(vec3 P)
{
   vec4 shadowMapSpacePos = lightSpaceMatrix * vec4(P, 1);
   shadowMapSpacePos.xyz = shadowMapSpacePos.xyz * 0.5 + 0.5;

   float shadowDepth = texture(ShadowMap, shadowMapSpacePos.xy).r;
   return shadowDepth + 0.01f <= clamp(shadowMapSpacePos.z, -1, 1) ? 0.0 : 1.0;
}

vec3 GetLight(out vec3 lightRadiance, vec3 P)
{
   bool positional = lightRadius > 0;

   lightRadiance = lightColor;

   // Modulate lightRadiance by distance attenuation (only for positional lights)
   float attenuation = positional ? GetAttenuation(P) : 1.0f;
   lightRadiance *= attenuation;

   // Modulate lightRadiance by shadow (only for directional light)
   float shadow = positional ? 1.0f : GetShadow(P);
   lightRadiance *= shadow;

   vec3 lightDirection = normalize(lightPosition - (positional ? P : vec3(0.0f)));
   return lightDirection;
}


void main()
{
   // Compute texture coordinates from the projected position
   vec2 texCoords = projPosition.xy / projPosition.w; 
   texCoords = texCoords * 0.5f + 0.5f;

   // Read depth buffer
   float depth = texture(DepthBuffer, texCoords).x;
   vec3 P = ReconstructPosition(projPosition, depth);

   // Read normal
   vec2 normalMap = texture(NormalGBuffer, texCoords).xy;
   vec3 N = ReconstructNormal(normalMap);

   // Read albedo
   vec3 albedo = texture(AlbedoGBuffer, texCoords).rgb;

   // Read specular
   vec4 others = texture(OthersGBuffer, texCoords);
   float roughness = others.r;
   float metalness = others.g;

   // Get light direction and radiance
   vec3 lightRadiance = vec3(0);
   vec3 L = GetLight(lightRadiance, P);

   // Get view direction in view space
   vec3 V = normalize(-P.xyz);

   // Get half vector
   vec3 H = normalize(L + V);

   // Compute diffuse lighting
   vec3 diffuse = GetLambertianDiffuseLighting(albedo);
   diffuse = mix(diffuse, vec3(0), metalness);

   // Compute specular lighting
   vec3 specular = GetCookTorranceSpecularLighting(N, L, V, roughness);

   // Compute Fresnel
   vec3 F0 = vec3(0.04f);
   F0 = mix(F0, albedo, metalness);
   vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));

   // Divide the incoming light between diffuse and specular, depending on Fresnel
   vec3 lighting = mix(diffuse, specular, F);

   // Apply the light
   lighting *= lightRadiance * max(dot(N, L), 0.0);

   FragColor = vec4(lighting, 1.0f);
}
