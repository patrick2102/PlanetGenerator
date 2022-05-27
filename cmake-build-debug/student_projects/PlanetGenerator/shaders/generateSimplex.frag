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

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;

in vec4 worldPos;
in vec3 worldNormal;
in vec3 TexCoords;
in float height;

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

/*
   if(height >= 0.05 && height <= 0.1)
   {
      float min = 0.05;
      float max = 0.1;
      float i = (height-min)/(max-min);
      return mix(i, water, dirt);
   }
   float min = 0.1;
   float max = 0.2;

   height = clamp(height, min, max);

   float i = (height-min)/(max-min);
   return mix(height, dirt, ice);
*/

}

void main()
{
   //vec3 albedo =

   /*
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
   */

   //FragColor = vec4(ambient + (diffuse + specular) * attenuation, 1.0);

   //FragColor = texture(surfaceTexture, TexCoords);

   //FragColor = texture(surfaceTexture, TexCoords);

   vec3 surfaceColor = surfaceColor();

   FragColor = vec4(surfaceColor, 1);

   //FragColor = vec4(TexCoords, 1);
}
