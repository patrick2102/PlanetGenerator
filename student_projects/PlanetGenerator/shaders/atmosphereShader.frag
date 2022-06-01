#version 330 core

uniform vec3 camFront; // so we can compute the view vector
uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

uniform mat4 localSpace; // represents model coordinates in the local coord space

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;
uniform float lightIntensity;
uniform float lightRadius;

//uniform vec3 center;
//uniform float inner_radius;
//uniform float outer_radius;
const float inner_radius = 1.0f;
const float outer_radius = 1.5f;
const vec3 center = vec3(0.0);

const float H0_ray = 0.05;
const int sample_count = 10;
const vec3 k_ray = vec3( 3.8, 13.5, 33.1 ); //Amount of light scatter

// math const
const float PI = 3.14159265359;
const float MAX = 10000.0;

// scatter const
//const float R_INNER = 0.5;
//const float R = R_INNER + 0.5;

const int in_scatter = 8;
const int out_scatter = 80;

in vec4 worldPos;
in vec3 worldNormal;
in vec3 atmosColor;
in vec3 localPos;

void main()
{
   //vec3 lightColor = atmosColor;
   //vec3 lightColor = vec3(1.0f);
   //vec3 lightColor = atmosphereScatter();
   //vec3 lightColor = atmosphereScatter2();
   //FragColor = vec4( pow( lightColor, vec3( 1.0 / 2.2 ) ), 1.0 );
   //FragColor = vec4(1.0f);

   //vec3 lightColor = atmosColor;
   //FragColor = vec4(lightColor, 1.0f);

   //vec3 lightColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ));
   vec3 lightColor = atmosColor;
   FragColor = vec4(lightColor, 1.0f);
}
