#version 330 core

// material textures
uniform sampler2D SourceTexture;

// TODO 9.6 : Add uniforms here
uniform vec2 scale;
uniform vec3 outlineColor;
uniform float distance;

// variables from vertex shader
in vec2 textureCoordinates;

// output color of this fragment
out vec4 FragColor;


const vec2 offsets[4] = vec2[](vec2(-1.25f, 0.0f), vec2(1.25f, 0.0f), vec2(0.0f, -1.25f),  vec2(0.0f, 1.25f));
const float weights[4] = float[](0.25f, 0.25f, 0.25f, 0.25f);


// TODO 9.6 : Read from the texture at the texture coordinates plus the offset, and process depth (you can return it unmodified at first, and then try different combinations)
float SampleDepth(vec2 offset)
{
   float depth = texture(SourceTexture, textureCoordinates + offset).r;
   depth = depth * 2 - 1;

   return depth * depth;
}


// TODO 9.6 : Sample depth with the provided offset and compare the obtained depth with the reference depth. Return the difference (only positive values)
float DetectEdge(vec2 offset, float depthRef)
{
   float depth = SampleDepth(offset);

   return abs(depth - depthRef) / distance;
}

void main()
{
   // TODO 9.6 : Sample depth with no offset (0,0). This will be the reference depth
   float depth = SampleDepth(vec2(0));

   float outlineIntensity = -0.1f;

   for(int i = 0; i < 4; ++i)
   {
      // TODO 9.6 : Fix the arguments to the DetectEdge function
      // You need to use an offset from the array provided, multiplied by the scale uniform, as first argument; and the reference depth as second
      outlineIntensity += DetectEdge(offsets[i] * scale, depth) * weights[i];
   }

   // TODO 9.6 : Return the outline color and outline intensity
   FragColor = vec4(outlineColor, clamp(outlineIntensity, 0, 1));
}
