#version 330 core

in vec3 worldPos;
in vec3 normal;

out vec4 FragColor;

uniform samplerCube environmentMap;
uniform vec3 cameraPos;

uniform float EtaR;
uniform float EtaG;
uniform float EtaB;
uniform float F0;
uniform int effectType;

float fresnelSchlick(float cosTheta, float f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 N = normalize(normal);
    vec3 V = normalize(cameraPos - worldPos); 

    // 1. Reflection
    
    vec3 R = texture(environmentMap, refelct(N,V)).rgb;
    vec3 reflColor = vec4(R, 1.0);

    // 2. Refraction (Chromatic Dispersion)
    vec3 T_R = refract(-V, N, EtaR);
    vec3 T_G = refract(-V, N, EtaG);
    vec3 T_B = refract(-V, N, EtaB);

    vec3 refrColor;
    refrColor.r = texture(environmentMap, T_R).r;
    refrColor.g = texture(environmentMap, T_G).g;
    refrColor.b = texture(environmentMap, T_B).b;

    // 3. Fresnel Logic
    float cosTheta = clamp(dot(N, V), 0.0, 1.0);
    float F = fresnelSchlick(cosTheta, F0);

    vec3 finalColor;
    if(effectType == 0)
    {
        finalColor = reflColor;
    }
    else if (effectType == 1)
    {
        finalColor = refrColor;
    }
    else
    {
        finalColor = mix(refrColor, reflColor, F);
    }

    FragColor = vec4(finalColor, 1.0);
}