#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform vec3 center;
uniform vec3 inner_radius;
uniform vec3 outer_radius;

const float H0_ray = 0.05;
const int sample_count = 10;

const int in_scatter = 1;
const int out_scatter = 8;

out vec3 atmosColor;

vec2 nearIntersection(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(sphere_origin, sphere_origin) + dot(origin, origin) - 2.0f*dot(sphere_origin, origin) - (r*r);
   float d = sqrt(b*b - 4.0f*a*c);

   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}

//exp(-h/H0), h = height, H0 = height with average density
float outScattering(float h)
{
   return exp(-max(h, 0.0) / H0_ray);
}

//rayleigh with g=0
//3*(1-g^2)/(2*(2+g^2))*(1+cos(angle)^2)/(1+g^2-2*g*cos(angle))^(3/2)
float rayleighPhase(float angle)
{
   return (3/4)*(1+(cos(angle)*cos(angle)));
}

/*
float density( vec3 p, float ph ) {
   return exp( -max( length( p ) - inner_radius, 0.0 ) / ph );
}
*/

vec3 atmosphereScatter()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);
   vec3 L = normalize(sunPosition - P.xyz);
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);

   float nearInter = nearIntersection(sunPosition, L);

   float len = ( e.y - e.x ) / float( NUM_IN_SCATTER );

   for(int i = 0; i < sample_count; i++)
   {

   }
}

void main()
{
   vec3 atmosColor = atmosphereScatter();

   vec3 localPos = vertex;
   vec4 P = model * vec4(localPos, 1.0);

   gl_Position = viewProjection * P;
}