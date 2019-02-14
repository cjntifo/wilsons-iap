#version 430 core

struct Material 
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float     shininess;
};  

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
//uniform int NR_POINT_LIGHTS;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform sampler2D texture1;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{             
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);  	
	
	//color = texture(texture1, TexCoords);
	//color = texture(texture1, TexCoords) + vec4(result, 1.0);
	//color = texture(texture1, TexCoords) * vec4(result, 1.0); 
	color = vec4(result, 1.0); 
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
	
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoords));
	
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
	
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoords));
	
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}