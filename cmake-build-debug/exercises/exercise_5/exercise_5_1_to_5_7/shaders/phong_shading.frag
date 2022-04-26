#version 330 core


uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// TODO exercise 5.4 setup the 'uniform' variables needed for lighting
// light uniform variables
uniform float ambientLightIntensity;
uniform float light1Intensity;
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform float light2Intensity;
uniform vec3 light2Position;
uniform vec3 light2Color;


// material properties
uniform float ambientReflectance;
uniform vec3 reflectionColor;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

// TODO exercise 5.4 add the 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec3 position;
in vec3 newNormal;

void main()
{

   // TODO exercise 5.4 - phong shading (i.e. Phong reflection model computed in the fragment shader)
   // ambient component
   vec3 ambient = ambientLightIntensity * ambientReflectance * reflectionColor;

   // diffuse component for light 1
   vec3 L1 = normalize(light1Position-position);
   vec3 light1Diffuse = diffuseReflectance * (dot(newNormal, L1)) * (light1Color * reflectionColor);

   // specular component for light 1
   vec3 V = normalize(camPosition-position);
   vec3 H1 = normalize(L1 + V);
   float light1Specular = specularReflectance * pow((dot(newNormal, H1)), specularExponent);


   // TODO exercuse 5.5 - attenuation - light 1
   float light1Attenuation = 1 / pow(distance(position, light1Position), 2);

   // TODO exercise 5.6 - multiple lights, compute diffuse, specular and attenuation of light 2
   vec3 L2 = normalize(light2Position-position);
   vec3 H2 = normalize(L2 + V);
   vec3 light2Diffuse = diffuseReflectance * (dot(newNormal, L2)) * light2Color * reflectionColor;
   float light2Specular = specularReflectance * pow((dot(newNormal, H2)), specularExponent);

   float light2Attenuation = 1 / pow(distance(position, light2Position), 2);

   // TODO compute the final shaded color (e.g. add contribution of the attenuated lights 1 and 2)

   vec3 light1 = (light1Intensity  * (light1Diffuse  + light1Specular));
   vec3 light2 = (light2Intensity * (light2Diffuse + light2Specular));

   vec3 reflection = ambient + light1 + light2 + light1Attenuation + light2Attenuation;

   // TODO set the output color to the shaded color that you have computed
   FragColor = vec4(reflection, 1.0);
}
