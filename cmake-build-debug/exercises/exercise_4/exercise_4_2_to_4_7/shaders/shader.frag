#version 330 core

in float age;

out vec4 fragColor;

void main()
{
    // remember to replace the default output (vec4(1.0,1.0,1.0,1.0)) with the color and alpha values that you have computed
    vec2 pc = gl_PointCoord;
    float alpha = 1 - smoothstep(0.0, 1.0, ((distance(vec2(0.5,0.5), pc))/distance(vec2(0.5,0.5), vec2(0.5, 0.0))));

    float red = 1.0;
    float green = mix(1.0, 0.5, (age/10.0));
    float blue = mix(0.05, 0.01, (age/10.0));

    if(age > 10.0)
    {
        red = 0.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    fragColor = vec4(red, green, blue, pow(alpha, 2.0));
}