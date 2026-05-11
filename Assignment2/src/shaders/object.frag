#version 330 core

in vec3 worldPos;
in vec3 normal;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

uniform float ior;
uniform float dispersion;
uniform float reflectivity;
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
    float eta = 1.0 / ior;

    vec3 finalColor;

    if (effectType == 0) 
    {
        // Pure Reflection
        vec3 R = reflect(I, N);
        finalColor = texture(skybox, R).rgb;
    }
    else if (effectType == 1) 
    {
        // Pure Refraction 
        vec3 T = refract(I, N, eta); // Use green channel eta as base
        finalColor = texture(skybox, T).rgb;
    }
    else if (effectType == 2) 
    {
        float ratioR = 1.0 / (ior - dispersion);
        float ratioG = 1.0 / ior;
        float ratioB = 1.0 / (ior +dispersion);

        // Chromatic Dispersion 
        vec3 T_R = refract(I, N, ratioR);
        vec3 T_G = refract(I, N, ratioG);
        vec3 T_B = refract(I, N, ratioB);

        vec3 refrColor;
        refrColor.r = texture(skybox, T_R).r;
        refrColor.g = texture(skybox, T_G).g;
        refrColor.b = texture(skybox, T_B).b;

        finalColor = refrColor;
    }
    else // effectType == 3 
    {
        // Fresnel Blend (reflection + refraction with chromatic dispersion)
        vec3 R = reflect(I, N);
        vec3 T = refract(I, N, eta); 

        vec3 reflColor = texture(skybox, R).rgb;
        vec3 refrColor = texture(skybox, T).rgb;

        float F = fresnelSchlick(I, N, reflectivity);
        
        finalColor = mix(refrColor, reflColor, F);
        
    }

    FragColor = vec4(finalColor, 1.0);
}