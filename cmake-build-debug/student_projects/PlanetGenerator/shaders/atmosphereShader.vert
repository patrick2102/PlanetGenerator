#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform vec3 camPosition; // so we can compute the view vector
uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 localSpace; // represents model coordinates in the local coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform vec3 sunPosition;

//uniform vec3 center;
uniform float inner_radius;
uniform float outer_radius;

const float H0_ray = 0.05;
const int sample_count = 10;
const vec3 k_ray = vec3( 3.8, 13.5, 33.1 ); //Amount of light scatter

// math const
const float PI = 3.14159265359;
const float MAX = 10000.0;

const int in_scatter = 8;
const int out_scatter = 80;

out vec4 worldPos;
out vec3 worldNormal;

out vec3 atmosColor;
out vec3 localPos;

//exp(-h/H0), h = height, H0 = height with average density
//Used to find the density of atmosphere at h height
float outScattering(float h)
{
   return exp(-max(h, 0.0) / H0_ray);
}

float optic( vec3 p, vec3 q) {
   vec3 s = ( q - p ) / float( out_scatter );
   vec3 v = p + s * 0.5;

   float sum = 0.0;
   for ( int i = 0; i < out_scatter; i++ ) {
      vec3 center = vec3(0.0f);
      float h = distance(v, center) - inner_radius;
      sum += outScattering(h);
      v += s;
   }
   sum *= length( s );

   return sum;
}

//3*(1-g^2)/(2*(2+g^2))*(1+cos(angle)^2)/(1+g^2-2*g*cos(angle))^(3/2)
//Used to find the amount of light scattered towards the camera position
float rayleighPhase(float angle)
{
   //return (3/4)*(1+(cos(angle)*cos(angle)));
   return (3.0/4.0)*(1.0+angle);
}

vec2 sphereIntersections(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(origin, origin) - (r*r);
   float d = sqrt(b*b - 4.0f*a*c);

   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}

vec3 atmosphereScatter()
{
   vec3 P = vertex;
   vec3 C = (localSpace * vec4(camPosition,1)).xyz;
   vec3 V = (P - C);
   //vec3 L = vec4(model vec3(0,0,1.0));
   //vec3 L = vec3(-1,0,0.0);
   //vec3 L = normalize((localSpace * vec4(sunPosition,1)).xyz - P);
   vec3 L = normalize(sunPosition - P);
   //vec3 L = normalize(sunPosition);
   //vec3 L = normalize(V);
   vec3 dir = normalize(V);
   //vec3 dir = vec3(0.0,-1.6,-5.0);
   //vec3 eye = vec3(0.0,1.6,5.0);
   vec3 eye = C;

   vec2 intersect = sphereIntersections(eye, dir, outer_radius);

   /*
   if(intersect.x <= 0)
      return vec3(0.0f);
   */

   vec2 innerIntersection = sphereIntersections(eye, dir, inner_radius);
   intersect.y = min(intersect.y, innerIntersection.x);

   float len = (intersect.y - intersect.x)/float(sample_count);

   vec3 s = dir * len;

   vec3 sum_ray = vec3( 0.0 );

   float n_ray0 = 0.0;

   vec3 v = eye + dir * ( intersect.x + len * 0.5 );

   for(int i = 0; i < sample_count; i++)
   {
      //float h = max(0.0, distance(v, center) - inner_radius);

      vec3 c2 = vec3(0,0,0);

      //float h = max(0.0, distance(v, c2) - inner_radius);
      float h = max(0.0, distance(v, c2) - inner_radius);
      //float h = max(0.0, 1.0f - inner_radius);
      //float h = 0.1;
      float d_ray = outScattering(h) * len;

      n_ray0 += d_ray;

      vec2 f = sphereIntersections( v, L, outer_radius );

      vec3 u = v + L * f.y;

      float n_ray1 = optic(v, u);

      vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray);

      sum_ray += d_ray * att;
      //sum_ray += d_ray;
      v += s;
   }

   float c  = dot( dir, -L );
   float cc = c * c;
   vec3 scatter = sum_ray * k_ray * rayleighPhase( cc );

   return 1.0 * scatter;
}

void main()
{
   //atmosColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ) );
   vec3 localPos = vertex;
   localPos = localPos * outer_radius;
   vec4 P = model * vec4(localPos, 1.0);

   //vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   localPos = vertex;
   //localNormal = normal;
   //atmosColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ) );
   //worldPos = P;
   //worldNormal = N;

   gl_Position = viewProjection * P;
}