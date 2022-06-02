#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;
uniform float lightIntensity;
uniform float lightRadius;

// material properties
uniform vec3 reflectionColor;
uniform float diffuseReflectance;
uniform float roughness;
uniform float metalness;

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;

in vec4 worldPos;
in vec3 worldNormal;
in float height;
in vec3 outColor;
in vec3 localPos;
in vec3 localNormal;

const float PI = 3.14159265359;


/*
float GetTemperature()
{
   vec3 P = worldPos.xyz;

   float distToStar = length(P - sunPosition);
}
*/

vec3 surfaceColor(float height)
{
   vec3 ice = vec3(1.0f);
   vec3 dirt = vec3(0.5, 0.25, 0.1);
   vec3 grass = vec3(0.0, 0.25, 0.0);
   vec3 water = vec3(0.004, 0.086, 0.2f);
   //vec3 water = vec3(1.0, 0.086, 0.2f);

   float min = 0.000f;

   // Underwater:
   if(height <= min)
   return water;

   // Coastline:
   float max = 0.0025f;
   if(min <= height && height <= max)
   {
      float i = (height-min)/(max-min);
      return  mix(water, dirt, i);
   }

   // Moving inlands
   min = max;
   max = 0.005;
   if(min <= height  && height <= max)
   {
      float i = (height-min)/(max-min);
      return  mix(dirt, outColor, i);
   }

   // Inland
   min = max;
   max = 0.025;
   if(min <= height && height <= max)
   {
      return outColor;
   }

   // Mountains:
   min = max;
   max = 0.05;
   float h = clamp(height, min, max);
   float i = (h-min)/(max-min);
   return mix(outColor, ice, i);
}

float GetAttenuation(vec4 P)
{
   float distToLight = distance(sunPosition, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);

   //float falloff = smoothstep(lightRadius, lightRadius*0.5f, distToLight);

   return attenuation;
}

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

float GeometrySmith(vec3 N, vec3 V, vec3 L, float a, float rough)
{
   float NdotV = max(dot(N, V), 0.0);
   float NdotL = max(dot(N, L), 0.0);
   float ggx2  = GeometrySchlickGGX(NdotV, rough);
   float ggx1  = GeometrySchlickGGX(NdotL, rough);

   return ggx1 * ggx2;
}

vec3 GetCookTorranceSpecularLighting(vec3 N, vec3 L, vec3 V, float rough)
{
   vec3 H = normalize(L + V);

   // Remap alpha parameter to roughness^2
   float a = rough * rough;

   float D = DistributionGGX(N, H, a);
   float G = GeometrySmith(N, V, L, a, rough);

   float cosI = max(dot(N, L), 0.0);
   float cosO = max(dot(N, V), 0.0);

   // Important! Notice that Fresnel term (F) is not here because we apply it later when mixing with diffuse
   float specular = (D * G) / (4.0f * cosO * cosI + 0.0001f);

   return vec3(specular);
}

vec3 GetLambertianDiffuseLighting(vec3 albedo)
{
   vec3 diffuse = diffuseReflectance * albedo;

   diffuse /= PI;

   return diffuse;
}

vec3 OrenNayar(vec3 N, vec3 L, vec3 V, vec3 albedo, float rough)
{
   vec3 diffuse = diffuseReflectance * albedo;
   diffuse /= PI;

   float angleLN = acos(dot(N,L));
   float angleNV = acos(dot(N,V));


   float alpha = max(angleNV, angleLN);
   float beta = min(angleNV, angleLN);
   float gamma = cos(angleNV - angleLN);

   float r2 = rough * rough;
   float A = 1 - 0.5*(r2/(r2+0.33));
   float B = 0.45*(r2/(r2+0.33));

   diffuse *= (A+B*max(0, gamma)*sin(alpha)*tan(beta));

   float diffuseModulation = max(dot(N, L), 0.0);
   //diffuse *= diffuseModulation;
   return diffuse;
}

vec3 PBR()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);
   vec3 L = normalize(sunPosition - P.xyz);
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);

   float displace = Displacement(localPos);
   vec3 albedo = surfaceColor(displace);


   //vec3 grey = vec3(0.5, 0.5, 0.5);
   //vec3 albedo = vec3(grey * displace);
   //float displace = Displacement(vertex);
   //localPos = localPos + (normal * displace);
   //albedo += vec3(height);

   //vec3 diffuse = OrenNayar(N, L, V, albedo);
   vec3 diffuse = albedo;
   vec3 specular = GetCookTorranceSpecularLighting(N, L, V);

   vec3 lightRadiance = sunColor;
   float attenuation = GetAttenuation(P);
   lightRadiance *= attenuation;
   lightRadiance *= lightIntensity;
   //lightRadiance *= max(dot(N, L), 0.0);

   vec3 F0 = vec3(0.02f);
   F0 = mix(F0, albedo, metalness);
   diffuse = mix(diffuse, vec3(0), metalness);
   vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));

   vec3 directLight = mix(diffuse, specular, F);
   directLight *= lightRadiance;

   //vec3 light = directLight;
   vec3 light = diffuse;
   return light;
}

void main()
{
   vec3 lightColor = PBR();
   FragColor = vec4(lightColor, 1);
}
