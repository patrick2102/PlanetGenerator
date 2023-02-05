#version 330 core
layout(location = 0) out vec4 color;

uniform samplerCube tex;
uniform vec3 tex_size;

in vec3 pos;

void main()
{
   //vec4 in_color = texture(tex, gl_FragCoord.xyz / tex_size);
   //vec4 in_color = gl_FragCoord;
   vec4 in_color = vec4(0.0f);
   color = vec4(1.0f);
}
