#version 330 core

in vec3 worldPos;
in vec3 normal;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

uniform float EtaR;
uniform float EtaG;
uniform float EtaB;
uniform float F0;
uniform int effectType; // 0: Reflection, 1: Refraction, 2: Chromatic, 3: Fresnel

float fresnelSchlick(vec3 I, vec3 N, float F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - max(dot(-I, N), 0.0), 5.0);
}

void main()
{
    vec3 N = normalize(normal);
    vec3 I = normalize(worldPos - cameraPos); 

    // Calculate fresnel term (used for multiple effects)
    float F = fresnelSchlick(I, N, F0);

    vec3 finalColor;

    if (effectType == 0) 
    {
        // Pure Reflection
        vec3 R = reflect(I, N);
        finalColor = texture(skybox, R).rgb;
    }
    else if (effectType == 1) 
    {
        // Pure Refraction (single wavelength)
        vec3 T = refract(I, N, EtaG); // Use green channel eta as base
        finalColor = texture(skybox, T).rgb;
    }
    else if (effectType == 2) 
    {
        // Chromatic Dispersion (separate wavelengths)
        vec3 T_R = refract(I, N, EtaR);
        vec3 T_G = refract(I, N, EtaG);
        vec3 T_B = refract(I, N, EtaB);

        vec3 refrColor;
        refrColor.r = texture(skybox, T_R).r;
        refrColor.g = texture(skybox, T_G).g;
        refrColor.b = texture(skybox, T_B).b;

        finalColor = refrColor;
    }
    else // effectType == 3 or default
    {
        // Fresnel Blend (reflection + refraction with chromatic dispersion)
        vec3 R = reflect(I, N);
        vec3 reflColor = texture(skybox, R).rgb;

        float eta = 1.0/1.52;
        vec3 T = refract(I, N, eta); 
        vec3 refrColor = texture(skybox, T).rgb;

        float F = fresnelSchlick(I, N, F0);
        
        finalColor = mix(refrColor, reflColor, F);
        
    }

    FragColor = vec4(finalColor, 1.0);
}