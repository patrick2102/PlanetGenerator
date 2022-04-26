#version 330 core

// material textures
uniform sampler2D SourceTexture;

// TODO 9.4 : Add uniforms here




// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;


float GetLuminance(vec3 color)
{
   return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main()
{
   vec3 color = texture(SourceTexture, textureCoordinates).rgb;

   // TODO 9.4 : Compute the luminance


   // TODO 9.4 : Divide the color by the luminance


   // TODO 9.4 : Substract the threshold


   // TODO 9.4 : Clamp the value between 0 and max intensity


   // TODO 9.4 : Apply scale to the resulting luminance


   // TODO 9.4 : Return the color multiplied by the luminance
   FragColor = vec4(color, 1.0f);
}
