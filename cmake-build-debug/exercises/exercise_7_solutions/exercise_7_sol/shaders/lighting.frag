#version 330 core

// transform matrices
uniform mat4 invProjection; // transform from clip space to view space

// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightRadius;

// g-buffers
uniform sampler2D AlbedoGBuffer;
uniform sampler2D NormalGBuffer;
uniform sampler2D OthersGBuffer;
uniform sampler2D DepthBuffer;

in vec4 projPosition;

out vec4 FragColor; // the output color of this fragment

void main()
{
   // Compute texture coordinates from the projected position
   vec2 texCoords = projPosition.xy / projPosition.w; 
   texCoords = texCoords * 0.5f + 0.5f;



   // Computing position in VIEW space:

   // Read depth buffer and change to range [-1, 1]
   float depth = texture(DepthBuffer, texCoords).x * 2 - 1;

   // Reconstruct clipPosition from projPosition(X,Y) and depth (Z)
   vec3 clipPosition = vec3(projPosition.xy / projPosition.w, depth);

   // Multiply clipPosition by inverse projection matrix to change to view space
   vec4 P = invProjection * vec4(clipPosition, 1.0f);

   // Divide by P.w after projecting
   P = P / P.w;



   // Read AlbedoGBuffer
   vec3 albedo = texture(AlbedoGBuffer, texCoords).rgb;



   // Read NormalGBuffer
   vec3 normal = texture(NormalGBuffer, texCoords).xyz;

   // TODO 7.4 Reconstruct Z component of the normal, knowing that the normal length is 1  (X*X + Y*Y + Z*Z = 1)
   normal.z = sqrt(1 - normal.x*normal.x - normal.y*normal.y);


   // Read NormalGBuffer
   vec4 others = texture(OthersGBuffer, texCoords);

   // TODO 7.5 : Unpack the remaining data - Remember to put values in their original range
   float ambientReflectance = others.x;
   float diffuseReflectance = others.y;
   float specularReflectance = others.z;
   float specularExponent = others.w * 100.0f;


   // TODO 7.6 : Compute the light vector (L) as usual. lightPosition is already in view space
   vec3 L = normalize(lightPosition - P.xyz);

   // TODO 7.6 : Compute the view vector (V) as usual, but taking into account that camera position in view space is (0, 0, 0)
   vec3 V = normalize(-P.xyz);

   // TODO 7.6 : Compute the lighting using the phong reflection model. You can get inspired by forward_shading.frag
   vec3 N = normal;

   vec3 ambient = ambientLightColor * ambientReflectance * albedo;

   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = lightColor * diffuseReflectance * diffuseModulation * albedo;

   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   vec3 specular = lightColor * specularReflectance * specModulation;

   float distToLight = distance(lightPosition, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);
   float falloff = smoothstep(lightRadius, lightRadius*0.5f, distToLight);
   attenuation *= falloff;

   vec3 lighting = ambient + (diffuse + specular) * attenuation;

   // Debug output for view space position
   //FragColor = P;

   // TODO 7.3 Output albedo to fragment color
   //FragColor = vec4(albedo, 1.0f);

   // TODO 7.4 Output normal to fragment color
   //FragColor = vec4(normal, 1.0f);

   // TODO 7.5 Output others to fragment color
   //FragColor = others;

   // TODO 7.6 Output lighting to fragment color
   FragColor = vec4(lighting, 1.0f);
}
