#version 330 core

// material textures
uniform sampler2D SourceTexture;

//TODO 9.4 : Add bloom texture sampler
uniform sampler2D BloomTexture;


//TODO 9.1 and 9.2 : Add uniforms here
uniform float exposure;

uniform float contrast = 1;
uniform float hueShift = 0;
uniform float saturation = 1;
uniform vec3 colorFilter = vec3(1);


// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;



float GetLuminance(vec3 color)
{
   return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

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

   //TODO 9.4 : Sample bloom texture and add the bloom to HDR color
   vec3 bloomColor = texture(BloomTexture, textureCoordinates).rgb;
   hdrColor += bloomColor;


   //TODO 9.1 : Apply tone mapping using the exposure uniform
   vec3 color = vec3(1.0) - exp(hdrColor * -exposure);


   //TODO 9.2 : Modify contrast
   color = (color - vec3(0.5f)) * contrast + vec3(0.5f);
   color = clamp(color, 0, 1);

   //TODO 9.2 : Modify hue
   vec3 hsvColor = RGBToHSV(color);
   hsvColor.x = fract(hsvColor.x + hueShift + 1);
   color = HSVToRGB(hsvColor);

   //TODO 9.2 : Modify saturation
   vec3 luminance = vec3(GetLuminance(color));
   color = (color - luminance) * saturation + luminance;
   color = clamp(color, 0, 1);

   //TODO 9.2 : Apply color filter
   color *= colorFilter;


   FragColor = vec4(color, 1.0f);
}
