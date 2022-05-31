#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

uniform int scale;
uniform float amplitude;
uniform float persistence;
uniform float lacunarity;
uniform int diameter;
uniform int iterations;

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

uniform int permTab[255*2];
uniform vec3 grad3[12];


//Terrain colors:
//vec3 water = vec3(0.004, 0.086, 0.2f);
//RGB values for terrain, alpha channel is for storing height for when terrain should start. Max 10 per biome type.
//uniform vec4 terrain[10];
//uniform float maxHeight;

in vec4 worldPos;
in vec3 worldNormal;
in float height;
in vec3 outColor;
in vec3 localPos;
in vec3 localNormal;

const float PI = 3.14159265359;
const float heightMultiplier = 20.0f;




/*
float GetTemperature()
{
   vec3 P = worldPos.xyz;

   float distToStar = length(P - sunPosition);
}
*/
/*
vec3 surfaceColor(float height)
{
   float min = 0;
   float max = 0;
   for(int i = 0; i < 10; i++)
   {
      vec3 t = terrain[i].rgb;
      float h = terrain[i].a;

      float m
   }
   return vec3(1); //Just return white if no terrain type was found
}
*/

float Simplex3D(vec3 coords)
{
   float F3 = 1.0/3.0;
   float G3 = 1.0/6.0;
   int ptSize = 255;

   float x = coords.x;
   float y = coords.y;
   float z = coords.z;

   float s = (x+y+z)*F3;
   int i = int(floor(x+s));
   int j = int(floor(y+s));
   int k = int(floor(z+s));

   float t = (i+j+k)*G3;
   float x0 = x - (i-t);
   float y0 = y - (j-t);
   float z0 = z - (k-t);

   int i1, j1, k1;
   int i2, j2, k2;
   if(x0>=y0) {
      if(y0>=z0)
      { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
      else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
      else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
   }
   else
   { // x0<y0
      if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
      else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
      else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
   }

   int ii = i & ptSize;
   int jj = j & ptSize;
   int kk = k & ptSize;

   int gi0 = permTab[ii+permTab[jj+permTab[kk]]] % 12;
   int gi1 = permTab[ii+i1+permTab[jj+j1+permTab[kk+k1]]] % 12;
   int gi2 = permTab[ii+i2+permTab[jj+j2+permTab[kk+k2]]] % 12;
   int gi3 = permTab[ii+1+permTab[jj+1+permTab[kk+1]]] % 12;

   float x1 = x0 - i1 + G3;
   float y1 = y0 - j1 + G3;
   float z1 = z0 - k1 + G3;

   float x2 = x0 - i2 + 2.0 * G3;
   float y2 = y0 - j2 + 2.0 * G3;
   float z2 = z0 - k2 + 2.0 * G3;

   float x3 = x0 - 1.0 + 3.0 * G3;
   float y3 = y0 - 1.0 + 3.0 * G3;
   float z3 = z0 - 1.0 + 3.0 * G3;

   int giArr[4] = int[](gi0, gi1, gi2, gi3);
   float xArr[4] = float[](x0, x1, x2, x3);
   float yArr[4] = float[](y0, y1, y2, y3);
   float zArr[4] = float[](z0, z1, z2, z3);

   float n = 0;
   for(int c = 0; c < 4; c++)
   {
      float _t = 0.5 - xArr[c]*xArr[c] - yArr[c]*yArr[c] - zArr[c]*zArr[c];
      if(_t >= 0)
      {
         _t *= _t;
         n += _t * _t * dot(grad3[giArr[c]], vec3(xArr[c], yArr[c], zArr[c]));
      }
   }
   return 32.0f * n;
}

float Displacement(vec3 coords)
{
   float freq = scale;
   float amp = amplitude;
   float res = 0;
   vec3 normal = normalize(localNormal);
   for(int i=0;i<iterations;i++){
      vec3 point = normalize(normal) * diameter;
      point += diameter;
      point /= freq;
      res += Simplex3D(point) * amp;

      freq /= lacunarity;
      amp *= persistence;
   }
   res /= 255;
   res = clamp(res, -1.0f, 1.0f);
   return res;
}

vec3 surfaceColor(float height)
{
   vec3 ice = vec3(1.0f);
   vec3 dirt = vec3(0.5, 0.25, 0.1);
   vec3 grass = vec3(0.0, 0.25, 0.0);
   vec3 water = vec3(0.004, 0.086, 0.2f);

   float min = 0.000f;

   // Underwater:
   if(height <= min)
      return water;

   // Coastline:
   float max = 0.003f;
   if(min <= height && height <= max)
   {
      float i = (height-min)/(max-min);
      return  mix(water, dirt, i);
   }

   // Moving inlands
   min = max;
   max = 0.006;
   if(min <= height  && height <= max)
   {
      float i = (height-min)/(max-min);
      return  mix(dirt, outColor, i) * (height*heightMultiplier);
   }

   // Inland
   min = max;
   max = 0.025;
   if(min <= height && height <= max)
   {
      return outColor * (height*heightMultiplier);
   }

   // Mountains:
   min = max;
   max = 0.05;
   float h = clamp(height, min, max);
   float i = (h-min)/(max-min);
   return mix(outColor, ice, i) * (height*heightMultiplier);
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

   //float tHeight = texture(surfaceTexture, TexCoords).r;
   //normal = normalize(normal * 2.0 - 1.0);

   //vec3 albedo = texture(texture_diffuse1, textureCoordinates).xyz;
   //vec3 albedo = texture()
   //vec3 albedo = surfaceColor();
   //albedo += vec3(height);
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

   vec3 F0 = vec3(0.01f);
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
