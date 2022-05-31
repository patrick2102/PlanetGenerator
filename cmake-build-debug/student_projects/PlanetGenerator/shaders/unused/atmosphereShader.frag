#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;
uniform float lightIntensity;
uniform float lightRadius;

in vec3 atmosColor;

void main()
{
   //vec3 lightColor = atmosColor;
   vec3 lightColor = atmosColor;
   FragColor = vec4(lightColor, 1);
   //FragColor = vec4(1.0f);
}
