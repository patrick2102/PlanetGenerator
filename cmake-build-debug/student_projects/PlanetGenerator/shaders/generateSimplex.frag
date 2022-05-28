#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;

// material properties
uniform vec3 reflectionColor;
uniform float ambientReflectance;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

uniform float roughness;
uniform float metalness;

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;

in vec4 worldPos;
in vec3 worldNormal;
in float height;

const float PI = 3.14159265359;

vec3 surfaceColor()
{
   vec3 water = vec3(0, 0, 0.8);
   vec3 ice = vec3(0.5, 0.5, 0.5);
   vec3 dirt = vec3(0.5, 0.25, 0.1);
   vec3 grass = vec3(0.0, 0.25, 0.0);

   if(height == 0)
      return water;

   float min = 0.00;
   float max = 0.01;

   if(height >= min && max <= 0.01)
   {
      float i = (height-min)/(max-min);
      return  mix(water, grass, i);
   }

   min = max;
   max = 0.1;
   float h = clamp(height, min, max);
   float i = (h-min)/(max-min);
   return mix(grass, ice, i);
}

vec3 Phong()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);

   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;

   vec3 L = normalize(sunPosition - P.xyz);
   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = sunColor * diffuseReflectance * diffuseModulation * reflectionColor;

   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);

   vec3 specular = sunColor * specularReflectance * specModulation;

   float distToSun = distance(sunPosition, P.xyz);
   //float attenuation = 1.0f / (distToSun * distToSun);
   float attenuation = 1.0f;
   return vec3(ambient + (diffuse + specular) * attenuation);
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

vec3 GetLambertianDiffuseLighting(vec3 albedo)
{
   vec3 diffuse = diffuseReflectance * albedo;

   diffuse /= PI;

   return diffuse;
}

vec3 OrenNayar(vec3 N, vec3 L, vec3 V, vec3 albedo)
{
   vec3 diffuse = diffuseReflectance * albedo;
   diffuse /= PI;

   float angleLN = acos(dot(N,L));
   float angleNV = acos(dot(N,V));


   float alpha = max(angleNV, angleLN);
   float beta = min(angleNV, angleLN);
   float gamma = cos(angleNV - angleLN);

   float r2 = roughness * roughness;
   float A = 1 - 0.5*(r2/(r2+0.33));
   float B = 0.45*(r2/(r2+0.33));

   diffuse *= (A+B*max(0, gamma)*sin(alpha)*tan(beta));

   float diffuseModulation = max(dot(N, L), 0.0);
   diffuse *= diffuseModulation;
   return diffuse;
}

vec3 PBR()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);
   vec3 L = normalize(sunPosition - P.xyz);
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);

   vec3 albedo = surfaceColor();
   albedo *= reflectionColor;

   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;
   vec3 diffuse = OrenNayar(N, L, V, albedo);
   vec3 specular = GetCookTorranceSpecularLighting(N, L, V);

   vec3 F0 = vec3(0.04f);
   F0 = mix(F0, albedo, metalness);

   diffuse = mix(diffuse, vec3(0), metalness);

   vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));
   vec3 FAmbient = FresnelSchlick(F0, max(dot(N, V), 0.0));

   vec3 indirectLight = mix(ambient, FAmbient, 0.5);
   vec3 directLight = mix(diffuse, specular, F);

   vec3 light = directLight + indirectLight;
   return light;
}

void main()
{
   vec3 lightColor = PBR();
   FragColor = vec4(lightColor, 1);
}
