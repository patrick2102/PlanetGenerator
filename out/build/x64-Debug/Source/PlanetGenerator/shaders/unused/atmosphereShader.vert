#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform vec3 camPosition; // so we can compute the view vector
uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform vec3 center;
uniform float inner_radius;
uniform float outer_radius;
uniform vec3 sunPosition;

const float H0_ray = 0.05;
const int sample_count = 10;
const vec3 k_ray = vec3( 3.8, 13.5, 33.1 ); //Amount of light scatter

const int in_scatter = 8;
const int out_scatter = 80;

out vec3 atmosColor;

vec2 sphereIntersections(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(center, center) + dot(origin, origin) - 2.0f*dot(center, origin) - (r*r);
   float d = sqrt(b*b - 4.0f*a*c);

   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}

//exp(-h/H0), h = height, H0 = height with average density
//Used to find the density of atmosphere at h height
float outScattering(float h)
{
   return exp(-max(h, 0.0) / H0_ray);
}

float density(vec3 p) {
   //return exp( -max( length( p ) - R_INNER, 0.0 ) / ph );
   return exp( -max( length( p ) - inner_radius, 0.0 ) / H0_ray );
}

//3*(1-g^2)/(2*(2+g^2))*(1+cos(angle)^2)/(1+g^2-2*g*cos(angle))^(3/2)
//Used to find the amount of light scattered towards the camera position
float rayleighPhase(float angle)
{
   //return (3/4)*(1+(cos(angle)*cos(angle)));
   return (3/4)*(1+(angle));
}

float optic( vec3 p, vec3 q) {
   vec3 s = ( q - p ) / float( out_scatter );
   vec3 v = p + s * 0.5;

   float sum = 0.0;
   for ( int i = 0; i < out_scatter; i++ ) {
      float h = distance(center, v)-inner_radius; // distance of v from ground
      sum += outScattering(h);
      v += s;
   }
   sum *= length( s );

   return sum;
}

vec3 atmosphereScatter()
{
   vec3 localPos = vertex;
   vec4 P = model * vec4(localPos, 1.0);
   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   //vec3 L = normalize(sunPosition - P.xyz);
   vec3 dir = normalize(camPosition - P.xyz);
   vec3 eye = vec3(0.0f, 0.0f, 3.0f);
   vec3 l = vec3(0.0f, 0.0f, 1.0f);
   //vec3 V = normalize(camPosition - P.xyz);
   vec3 V = normalize(camPosition - P.xyz);
   //vec3 H = normalize(L + V);

   //si = sphere intersections
   vec2 si = sphereIntersections(eye, dir, outer_radius);
   vec2 si2 = sphereIntersections(eye, dir, inner_radius);
   si.y = min(si.y, si2.x);

   //Set si.x to be the closest intersection, and si.y to the furthest.
   /*
   float minI = min(si.x, si.y);
   float maxI = max(si.x, si.y);
   si.x = minI;
   si.y = maxI;
   */

   vec3 sum_ray = vec3(0.0f);
   float n_ray0 = 0.0;

   // length of each segment we pass though the atmosphere with
   float len = (si.y - si.x) / float(in_scatter);

   vec3 s = dir * len;

   // position of ray from sun in atmosphere
   vec3 v = sunPosition + dir * (si.x + len * 0.5);

   //float overall_density = 0.0f;

   for(int i = 0; i < in_scatter; i++)
   {
      float d_ray = density( v) * len;
      //float h = distance(center, v)-inner_radius; // distance of v from ground
      //float density = outScattering(h) * len; // density of atmosphere times length of segment

      //overall_density += density;

      n_ray0 += d_ray;

      vec2 f = sphereIntersections( v, l, outer_radius );

      vec3 u = v + l * f.y;

      float n_ray1 = optic(v, u);

      vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray);

      sum_ray += d_ray * att;

      v += s;
   }

   float c  = dot(dir, -l);
   float cc = c * c;
   vec3 scatter = sum_ray * k_ray * rayleighPhase( cc );


   return 10.0 * scatter;
}

void main()
{


   vec3 localPos = vertex;
   vec4 P = model * vec4(localPos, 1.0);

   atmosColor = atmosphereScatter();
   //atmosColor = vec3(1.0f);
   gl_Position = viewProjection * P;
}