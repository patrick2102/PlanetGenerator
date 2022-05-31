#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 sunPosition;
uniform vec3 sunColor;
uniform float lightIntensity;
uniform float lightRadius;

uniform vec3 sphereOrigin;
uniform vec3 sphereRadius;

const float PI = 3.14159265359;
const int samples = 10;

in vec3 atmosColor;

vec3 scatter()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);
   vec3 L = normalize(sunPosition - P.xyz);
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);

   float nIntersection = nearIntersection(sunPosition, L);



   if (nIntersection >= 0.0f)
   {
      return vec3(0); //line didn't intersect with atmosphere
   }

   const float ph_ray = 0.05;
   float n_ray0 = 0.0;


   vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
   for(int i=0; i<samples; i++)
   {
      float d_ray = density(L, ph_ray) * 1.0f;
      n_ray0 += d_ray;

   }

   //eye = sunPosition // ray origin
   //dir = L // ray direction
   //l = L // sunlight dir
   //p = eye
   //R = outer radius
   //o = eye






   const float ph_ray = 0.05;
   const vec3 k_ray = vec3( 3.8, 13.5, 33.1 );
   vec3 sum_ray = vec3( 0.0 );
   float n_ray0 = 0.0;

   float len = ( e.y - e.x ) / float(10);
   vec3 s = dir * len;
   vec3 v = o + dir * ( e.x + len * 0.5 );

   for (int i = 0; i < 10; i++, v += s ) {
      float d_ray = density( v, ph_ray ) * len;

      n_ray0 += d_ray;

      vec2 f = ray_vs_sphere( v, l, R );
      vec3 u = v + l * f.y;

      float n_ray1 = optic( v, u, ph_ray );

      vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray - ( n_mie0 + n_mie1 ) * k_mie * k_mie_ex );

      sum_ray += d_ray * att;
      sum_mie += d_mie * att;
   }
}


void main()
{

   vec3 lightColor = scatter();
   FragColor = vec4(lightColor, 1);
}
