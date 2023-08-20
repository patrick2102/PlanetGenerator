#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec4 worldPos1;
out vec3 worldNormal1;
out float height1;
out vec3 outColor1;
out vec3 localPos1;
out vec3 localNormal1;

in vec4 worldPos;
in vec3 worldNormal;
in float height;
in vec3 outColor;
in vec3 localPos;
in vec3 localNormal;

void main() {
    worldPos1 = worldPos;
    worldNormal1    =  worldNormal;
    height1         =  height;
    outColor1       =  outColor;
    localPos1       =  localPos;
    localNormal1    =  localNormal;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}