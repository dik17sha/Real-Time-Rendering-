#version 330 core 

out vec4 FragColor;

in vec2 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos; 

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform bool useNormalMap;

void main()
{
    vec3 normal;

    if(useNormalMap)
    {
        normal = texture(texture_normal1, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);

    } else {
        normal = vec3(0.0, 0.0, 1.0);
    }

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    vec3 ambient = 0.4 * color;

    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * 1.2; 


    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);

}

