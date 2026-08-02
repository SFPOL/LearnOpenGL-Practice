#version 420 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;  
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    
    vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // combine results
    vec3 ambient = light.ambient * materialDiffuse;
    vec3 diffuse = light.diffuse * diff * materialDiffuse;
    vec3 specular = light.specular * spec * materialSpecular;

    return (ambient + diffuse + specular);
}

struct PntLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalcPointLight(PntLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * materialDiffuse;
    vec3 diffuse = light.diffuse * diff * materialDiffuse;
    vec3 specular = light.specular * spec * materialSpecular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

struct SpotLight {
    vec3 position;

    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    
    vec3 lightToFragVec = light.position - FragPos;
    float distance = length(lightToFragVec);
    vec3 lightDir = normalize(lightToFragVec);

    vec3 ambient = light.ambient * materialDiffuse;

    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * materialDiffuse);

    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * (spec * materialSpecular); 

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float intensity = 0.0;
    float cosTheta = dot(lightDir, normalize(-light.direction));
    if(cosTheta >= light.outerCutOff) {
        intensity = 1.0;
        if(cosTheta < light.cutOff) {
            float epsilon = light.cutOff - light.outerCutOff;
            intensity = clamp((cosTheta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    vec3 result = ambient + (diffuse + specular) * intensity;
    return result * attenuation;
}

#define NR_POINT_LIGHTS 4

uniform DirLight dirLight;
uniform PntLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;  

void main()
{

    vec3 materialDiffuse = vec3(texture(texture_diffuse1, TexCoords));
    vec3 materialSpecular = vec3(texture(texture_specular1, TexCoords));
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, materialDiffuse, materialSpecular);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, materialDiffuse, materialSpecular);
   
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, materialDiffuse, materialSpecular); 

    FragColor = vec4(result, 1.0);
}