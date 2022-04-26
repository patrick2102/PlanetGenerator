#version 330 core

// material textures
uniform sampler2D SourceTexture;

// TODO 9.6 : Add uniforms here
uniform float exposure;
uniform int steps;

// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;


vec3 RGBToHSV(vec3 rgb)
{
   vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);

   vec4 p = mix( vec4( rgb.bg, K.wz ), vec4( rgb.gb, K.xy ), step( rgb.b, rgb.g ) );
   vec4 q = mix( vec4( p.xyw, rgb.r ), vec4( rgb.r, p.yzx ), step( p.x, rgb.r ) );

   float d = q.x - min( q.w, q.y );

   float epsilon = 1.0e-10;

   return vec3( abs(q.z + (q.w - q.y) / (6.0 * d + epsilon)), d / (q.x + epsilon), q.x);
}

vec3 HSVToRGB( vec3 hsv )
{
   vec4 K = vec4( 1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0 );

   vec3 p = abs( fract( hsv.xxx + K.xyz ) * 6.0 - K.www );

   return hsv.z * mix( K.xxx, clamp(p - K.xxx, 0, 1), hsv.y );
}

void main()
{
   vec3 hdrColor = texture(SourceTexture, textureCoordinates).rgb;

   // TODO 9.6 : Apply tone-mapping as in exercise 9.1
   vec3 color = vec3(1.0) - exp(hdrColor * -exposure);

   // TODO 9.6 : convert the color in HSV format
   vec3 hsvColor = RGBToHSV(color);

   // TODO 9.6 : modify the value (V in HSV) to clamp the intensity as we did in exercise 6 cel-shading
   hsvColor.z = round(hsvColor.z * steps) / steps;

   // TODO 9.6 : convert the color back to RGB format
   color = HSVToRGB(hsvColor);

   FragColor = vec4(color, 1.0f);
}
