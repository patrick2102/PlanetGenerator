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

uniform vec3 center;
uniform float inner_radius;
uniform float outer_radius;

const float H0_ray = 0.01;
//const int sample_count = 100;
//const vec3 scatter_colors = vec3( 3.8, 13.5, 33.1 ); // Light spectrum colors scattered
const vec3 scatter_colors = vec3( 4.0, 13.0, 33.0 ); // Light spectrum colors scattered

// math const
const float PI = 3.14159265359;
const float MAX = 10000.0;

const float hdr = 0.5;

// scatter const
//const float R_INNER = 0.5;
//const float R = R_INNER + 0.5;

const int in_scatter_count = 10;
const int out_scatter_count = 10;

in vec4 worldPos;
in vec3 worldNormal;
in vec3 atmosColor;
in vec3 localPos;

//exp(-h/H0), h = height, H0 = height with average density
//Used to find the density of atmosphere at h height
float density(float h)
{
   return exp(-max(h, 0.0) / H0_ray);
}

//Light scattering from start point "sp" to the exit point of the atmosphere "ep".
float outScattering(vec3 sp, vec3 ep) {
   vec3 s_len = ( ep - sp ) / float(out_scatter_count); //Average segment from sp to ep
   vec3 p = sp + s_len * 0.5; //start from sp + midpoint on first segment

   float total_scatter = 0.0;
   for (int i = 0; i < out_scatter_count; i++ ) {
      float h = length(p) - inner_radius; //Height from point p to ground
      total_scatter += density(h);
      p += s_len;
   }
   total_scatter *= length(s_len);
   return total_scatter;
}

//3*(1-g^2)/(2*(2+g^2))*(1+cos(angle)^2)/(1+g^2-2*g*cos(angle))^(3/2)
//Used to find the amount of light scattered towards the camera position
float rayleighPhase(float angle)
{
   return (3.0/4.0)*(1.0+angle);
}

//Returns a vector containing the two intersections
//Math described in: http://paulbourke.net/geometry/circlesphere/
vec2 sphereIntersections(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(origin, origin) - (r*r);
   float d = b*b - 4.0f*a*c;
   d = sqrt(d);
   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}

vec3 atmosphereScatter()
{
   //P is the point on the outer radius,
   vec3 P = localPos;

   //We move the camera such that the coords of the center of the sphere can be treated as (0,0,0),
   //done to simplify the math in some later functions
   vec3 C = camPosition;
   C -= center;

   //Move the sun for the same reason as above.
   vec3 S = sunPosition;
   S -=center;

   vec3 V = normalize(P - C);
   vec3 L = normalize(S - P);
   //vec3 L = vec3(-1,0,0);

   //The intersections of the atmosphere, vec2 represent the two results from the ray entering and leaving the atmosphere
   vec2 intersect = sphereIntersections(C, V, outer_radius);

   //Check the inner radius to see if ray touches the ground or leaves the atmosphere.
   vec2 innerIntersection = sphereIntersections(C, V, inner_radius);

   //Set the endpoint of the ray intersection with the atmosphere to be the start of ground intersection.
   intersect.y = min(intersect.y, innerIntersection.x);

   //Length of each segment that we sample a scatter point from.
   float len = (intersect.y - intersect.x)/float(in_scatter_count);

   vec3 ray_color = vec3( 0.0 );

   float in_scatter = 0.0;

   //Sample point for sampling the atmosphere scattering. Start from the first intersection + segment length/2
   vec3 sample_point = C + V * ( intersect.x + len * 0.5 );

   for(int i = 0; i < in_scatter_count; i++)
   {
      //Height from ground. length of is used because we treat the sphere as having a center in (0,0,0).
      float h = max(0.0, length(sample_point) - inner_radius);

      //Calculate density at h height and add to in_scatter
      float d = density(h) * len;
      in_scatter += d;

      //Find point where ray leaves the atmosphere towards the light source.
      vec3 point_out = sample_point + L * sphereIntersections(sample_point, L, outer_radius).y;
      float out_scatter = outScattering(sample_point, point_out);

      //Calculate attenuation from both in and out scattering.
      vec3 att = exp( - ( in_scatter + out_scatter ) * scatter_colors);

      //ay_color += d * att;
      //ray_color += att/1000.0f;
      ray_color += d * att * scatter_colors;
      sample_point += (V * len);
   }
   //ray_color *= scatter_colors;

   float cos_angle  = dot( V, -L );
   vec3 scatter = ray_color;

   scatter *= rayleighPhase(cos_angle*cos_angle);

   return 0.001f * scatter;
}

void main()
{
   vec3 lightColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ) );
   FragColor = vec4(lightColor, 1.0f);
}
