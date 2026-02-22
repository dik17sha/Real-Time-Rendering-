#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;
uniform sampler2D texture_diffuse1;
uniform float lod_bias; // Add this new uniform

void main()
{
   
    FragColor = texture(texture_diffuse1, vTexCoord, lod_bias);
}