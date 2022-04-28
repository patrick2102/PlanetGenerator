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

in vec4 worldPos;
in vec3 worldNormal;

void main()
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
   float attenuation = 1.0f / (distToSun * distToSun);

   FragColor = vec4(ambient + (diffuse + specular) * attenuation, 1.0);
}
