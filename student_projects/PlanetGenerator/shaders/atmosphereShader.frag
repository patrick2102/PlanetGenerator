#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;
uniform float lightIntensity;
uniform float lightRadius;

uniform vec3 center;
uniform float inner_radius;
uniform float outer_radius;

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


//exp(-h/H0), h = height, H0 = height with average density
//Used to find the density of atmosphere at h height
float outScattering(float h)
{
   return exp(-max(h, 0.0) / H0_ray);
}

/*
float outScattering( vec3 p) {
   //return exp( -max( length( p ) - R_INNER, 0.0 ) / ph );
   return exp( -max( length( p ) - inner_radius, 0.0 ) / H0_ray );
}
*/

float optic( vec3 p, vec3 q) {
   vec3 s = ( q - p ) / float( out_scatter );
   vec3 v = p + s * 0.5;

   float sum = 0.0;
   for ( int i = 0; i < out_scatter; i++ ) {
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
/*
vec2 sphereIntersections(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(center, center) + dot(origin, origin) - 2.0f*dot(center, origin) - (r*r);
   float d = sqrt(b*b - 4.0f*a*c);

   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}
*/

vec2 sphereIntersections(vec3 origin, vec3 dir, float r)
{
   float a = dot(dir, dir);
   float b = 2.0f * dot(origin, dir);
   float c = dot(origin, origin) - (r*r);
   float d = sqrt(b*b - 4.0f*a*c);

   return vec2((-b -d)/(2.0f*a), (-b + d)/(2.0f*a));
}

vec3 IScatter(vec3 o, vec3 dir, vec2 e, vec3 l)
{
   vec3 sum_ray = vec3( 0.0 );
   float n_ray0 = 0.0;
   float len = ( e.y - e.x ) / float( in_scatter );

   vec3 s = dir * len;

   vec3 v = o + dir * ( e.x + len * 0.5 );

   for ( int i = 0; i < in_scatter; i++, v += s ) {
      float h = distance(v, center) - inner_radius;
      float d_ray = outScattering(h) * len;

      n_ray0 += d_ray;

      vec2 f = sphereIntersections( v, l, outer_radius );

      vec3 u = v + l * f.y;

      float n_ray1 = optic( v, u);

      vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray);

      sum_ray += d_ray * att;
   }

   float c  = dot( dir, -l );
   float cc = c * c;
   vec3 scatter = sum_ray * k_ray * rayleighPhase( cc );

   return 1.0 * scatter;
}

vec3 atmosphereScatter()
{
   vec4 P = worldPos;
   vec3 L = normalize(sunPosition - P.xyz);
   //vec3 V = normalize(camPosition - P.xyz);
   vec3 V = normalize(camPosition);
   //V+= P.xyz;

   vec3 dir = V;

   vec3 eye = camPosition.xyz;
   vec3 l = vec3(0, 0, 2);

   vec2 e = sphereIntersections( eye, dir, outer_radius );
   vec2 f = sphereIntersections( eye, dir, inner_radius );
   e.y = min( e.y, f.x );

   vec3 I = IScatter( eye, dir, e, l );

   return I;
}



void main()
{
   //vec3 lightColor = atmosColor;
   //vec3 lightColor = vec3(1.0f);
   vec3 lightColor = atmosphereScatter();
   FragColor = vec4( pow( lightColor, vec3( 1.0 / 2.2 ) ), 1.0 );
   //FragColor = vec4(1.0f);
}