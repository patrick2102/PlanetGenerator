in vec4 c0;
in vec4 c1;
vec3 v3Direction;

uniform vec3 v3LightDirection;
uniform float g;
uniform float g2;

void main()
{
   float fCos = dot(v3LightDirection, v3Direction) / length(v3Direction);
   float fCos2 = fCos * fCos;
   vec4 color = getRayleighPhase(fCos2) * c0 + getMiePhase(fCos, fCos2, g, g2) * c1;
   color.a = color.b;
   fragcolor = color;
}