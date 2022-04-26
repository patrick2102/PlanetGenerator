#version 330 core

// material textures
uniform sampler2D SourceTexture;

// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;


void main()
{
   FragColor = texture(SourceTexture, textureCoordinates);
}
