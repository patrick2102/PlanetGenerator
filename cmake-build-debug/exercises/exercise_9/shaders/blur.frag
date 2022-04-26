#version 330 core

// material textures
uniform sampler2D SourceTexture;

// uniforms
uniform vec2 blurScale;

// Offset (in pixels) where to sample the neighbors. We sample between texels to take advantage of the linear filtering
uniform float offsets[3] = float[](0.0, 1.3846153846, 3.2307692308);

// Weights of each sample. Notice that the first one gets added once, but the others are added twice, for positive and negative. Total sum is 1
uniform float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;


void main()
{
   // Sample the pixel at the center
   vec4 color = texture(SourceTexture, textureCoordinates) * weights[0];

   // Sample the pixel at the sides
   for (int i = 1; i < 3; i++)
   {
      vec2 texCoordOffset = blurScale * offsets[i];
      color += texture(SourceTexture, textureCoordinates + texCoordOffset) * weights[i];
      color += texture(SourceTexture, textureCoordinates - texCoordOffset) * weights[i];
   }

   FragColor = color;
}
