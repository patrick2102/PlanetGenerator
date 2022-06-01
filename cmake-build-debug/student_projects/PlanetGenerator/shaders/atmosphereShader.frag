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
//const float inner_radius = 0.05f;
//const float outer_radius = 1.0f;
//const vec3 center = vec3(0.0);

const float H0_ray = 0.05;
const int sample_count = 20;
const vec3 k_ray = vec3( 3.8, 13.5, 33.1 ); //Amount of light scatter

// math const
const float PI = 3.14159265359;
const float MAX = 10000.0;

const float hdr = 0.5;

// scatter const
//const float R_INNER = 0.5;
//const float R = R_INNER + 0.5;

const int in_scatter = 8;
const int out_scatter = 80;

in vec4 worldPos;
in vec3 worldNormal;
in vec3 atmosColor;
in vec3 localPos;

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

vec3 atmosphereScatter2()
{
   //P is the point on the outer radius,
   vec3 P = localPos;
   vec3 C = camPosition;
   C -= center;

   //We move the camera such that the coords of the center of the sphere can be treated as (0,0,0),
   //done to simplify the math in some later functions
   //C -= center;

   vec3 V = normalize(P - C);
   vec3 L = normalize(sunPosition - P);

   //The intersections of the atmosphere, vec2 represent the two results from the ray entering and leaving the atmosphere
   vec2 intersect = sphereIntersections(C, V, outer_radius);

   //Check the inner radius to see if ray touches the ground or leaves the atmosphere.
   vec2 innerIntersection = sphereIntersections(C, V, inner_radius);

   //Set the endpoint of the ray intersection with the atmosphere to be the start of ground intersection.
   intersect.y = min(intersect.y, innerIntersection.x);

   //Length of each segment that we sample a scatter point from.
   float len = (intersect.y - intersect.x)/float(sample_count);

   vec3 sum_ray = vec3( 0.0 );

   float n_ray0 = 0.0;

   //Sample point for sampling the atmosphere scattering. Start from the first intersection + segment length/2
   vec3 sample_point = C + V * ( intersect.x + len * 0.5 );

   for(int i = 0; i < sample_count; i++)
   {
      //Height from ground. length of is used because we treat the sphere as having a center in (0,0,0).
      float h = max(0.0, length(sample_point) - inner_radius);


      float d_ray = outScattering(h) * len;

      n_ray0 += d_ray;

      vec2 f = sphereIntersections( sample_point, L, outer_radius );

      vec3 u = sample_point + L * f.y;

      float n_ray1 = optic(sample_point, u);
      vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray);
      sum_ray += d_ray * att;
      sample_point += (V * len);
   }

   float c  = dot( V, -L );
   float cc = c * c;
   vec3 scatter = sum_ray * k_ray * rayleighPhase( cc );

   return 1.0 * scatter;
}

void trying()
{
   vec3 lightColor = atmosphereScatter2();
   FragColor = vec4(lightColor, 1.0f);
}

void main()
{
   if(true)
   {
      trying();
      return;
   }

   //vec3 lightColor = atmosColor;
   //vec3 lightColor = vec3(1.0f);
   //vec3 lightColor = atmosphereScatter();
   //vec3 lightColor = atmosphereScatter2();
   //FragColor = vec4( pow( lightColor, vec3( 1.0 / 2.2 ) ), 1.0 );
   //FragColor = vec4(1.0f);

   //vec3 lightColor = atmosColor;
   //FragColor = vec4(lightColor, 1.0f);

   //vec3 lightColor = pow(atmosphereScatter(), vec3( 1.0 / 2.2 ));
  // vec3 lightColor = atmosColor;

   vec3 P = localPos;
   vec3 C = camPosition;
   C -= center;
   vec3 V = normalize(P - C);
   vec3 L = normalize(sunPosition - P);

   float c  = dot( V, -L );
   float cc = c * c;
   vec3 lightColor = atmosColor;//*rayleighPhase(cc);
   //lightColor = 1.0 - exp(lightColor * -hdr);
   lightColor;// *= k_ray;

   FragColor = vec4(lightColor, 1.0f);
}
