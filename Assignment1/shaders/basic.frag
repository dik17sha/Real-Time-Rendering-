#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform int modelType; 


void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    //Declaring the variables here 
    float diff = 0.0;
    float spec = 0.0;
    float intensity = 0.0;
    float roughness = 0.5;
    float sigma2 = 0.0;
    float A = 0.0;
    float B = 0.0;

    //Blinn - Phong
    if(modelType == 0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        diff = max(dot(norm, lightDir), 0.0);
        spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        result = (diff + spec) * objectColor; 
    }

    // Toon Shading 
    else if(modelType == 1)
    {
        intensity = dot(lightDir, norm);
        float level = floor(intensity * 4.0) / 4.0;
        result = max(level, 0.1) * objectColor; 
    }

    //Oren-Nayar 
    else if(modelType == 2)
    {
        float LdotN = dot(lightDir, norm);
        float VdotN = dot(viewDir, norm);

        float cosThetaI = clamp(LdotN, 0.0, 1.0);
        float cosThetaR = clamp(VdotN, 0.0, 1.0);

        float thetaI = acos(cosThetaI);
        float thetaR = acos(cosThetaR);

        float alpha = max(thetaI, thetaR);
        float beta = min(thetaI, thetaR);

        sigma2 = roughness * roughness; 

        A = 1.0 - 0.5 * (sigma2/ (sigma2 + 0.33));
        B = 0.45 * (sigma2/ (sigma2 + 0.09));

        vec3 lightProj = lightDir - norm * LdotN;
        vec3 viewProj = viewDir - norm * VdotN;

        float cosPhiDiff = 0.0;
        if(length(lightProj) > 0.001 && length(viewProj) > 0.001)
        {
            cosPhiDiff = max(0.0, dot(normalize(lightProj), normalize(viewProj)));
        }

        float direct = (A + B * cosPhiDiff * sin(alpha) * tan(beta));

        result = objectColor * cosThetaI * direct; 
    }

    FragColor = vec4(result, 1.0);

    
}