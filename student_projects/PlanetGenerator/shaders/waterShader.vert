#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform int scale;
uniform float amplitude;
uniform float persistence;
uniform float lacunarity;
uniform int diameter;
uniform int iterations;

uniform mat4 model; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

uniform samplerCube surfaceTexture;
uniform samplerCube displacementMap;
uniform int permTab[255*2];
uniform vec3 grad3[12];

out vec4 worldPos;
out vec3 worldNormal;
out float height;

float Simplex3D(vec3 coords)
{
   float F3 = 1.0/3.0;
   float G3 = 1.0/6.0;
   int ptSize = 255;

   float x = coords.x;
   float y = coords.y;
   float z = coords.z;

   float s = (x+y+z)*F3;
   int i = int(floor(x+s));
   int j = int(floor(y+s));
   int k = int(floor(z+s));

   float t = (i+j+k)*G3;
   float x0 = x - (i-t);
   float y0 = y - (j-t);
   float z0 = z - (k-t);

   int i1, j1, k1;
   int i2, j2, k2;
   if(x0>=y0) {
      if(y0>=z0)
      { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
      else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
      else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
   }
   else
   { // x0<y0
      if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
      else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
      else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
   }

   int ii = i & ptSize;
   int jj = j & ptSize;
   int kk = k & ptSize;

   int gi0 = permTab[ii+permTab[jj+permTab[kk]]] % 12;
   int gi1 = permTab[ii+i1+permTab[jj+j1+permTab[kk+k1]]] % 12;
   int gi2 = permTab[ii+i2+permTab[jj+j2+permTab[kk+k2]]] % 12;
   int gi3 = permTab[ii+1+permTab[jj+1+permTab[kk+1]]] % 12;

   float x1 = x0 - i1 + G3;
   float y1 = y0 - j1 + G3;
   float z1 = z0 - k1 + G3;

   float x2 = x0 - i2 + 2.0 * G3;
   float y2 = y0 - j2 + 2.0 * G3;
   float z2 = z0 - k2 + 2.0 * G3;

   float x3 = x0 - 1.0 + 3.0 * G3;
   float y3 = y0 - 1.0 + 3.0 * G3;
   float z3 = z0 - 1.0 + 3.0 * G3;

   int giArr[4] = int[](gi0, gi1, gi2, gi3);
   float xArr[4] = float[](x0, x1, x2, x3);
   float yArr[4] = float[](y0, y1, y2, y3);
   float zArr[4] = float[](z0, z1, z2, z3);

   float n = 0;
   for(int c = 0; c < 4; c++)
   {
      float _t = 0.5 - xArr[c]*xArr[c] - yArr[c]*yArr[c] - zArr[c]*zArr[c];
      if(_t >= 0)
      {
         _t *= _t;
         n += _t * _t * dot(grad3[giArr[c]], vec3(xArr[c], yArr[c], zArr[c]));
      }
   }
   return 32.0f * n;
}

float Displacement(vec3 coords)
{
   float freq = scale;
   float amp = amplitude;
   float res = 0;
   for(int i=0;i<iterations;i++){
      vec3 point = normalize(normal) * diameter;
      point += diameter;
      point /= freq;
      res += Simplex3D(point) * amp;

      freq /= lacunarity;
      amp *= persistence;
   }
   res /= 255;
   res = clamp(res, -1.0f, 1.0f);
   return res;
}

void main() {

   vec3 localPos = vertex;

   float displace = Displacement(vertex);
   localPos = localPos + (normal * displace);

   vec4 P = model * vec4(localPos, 1.0);

   vec3 N = normalize(model * vec4(normal, 0.0)).xyz;

   worldPos = P;
   worldNormal = N;

   height = displace;
   gl_Position = viewProjection * P;
}