#version 430 core

struct Material 
{
	vec3 ambient;
    //vec3 specular; 
    sampler2D diffuse;
    //sampler2D specular;
    float     shininess;
};  

struct Light 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture1;

void main()
{             
    // Ambient
    //vec3 ambient = light.ambient * vec3(texture1, 0.8);
	//vec3 ambient = light.ambient;
	vec3 ambient = light.ambient * material.ambient;
	
	// Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // Specular
    //vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * (spec * material.specular); 
	
	// Attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    //ambient  *= attenuation;
	//diffuse  *= attenuation; 
	//specular  *= attenuation; 	
	
	//color = texture(texture1, TexCoords);
	color = texture(texture1, TexCoords); 
	//color = vec4(ambient + diffuse + specular, 1.0f); 
}