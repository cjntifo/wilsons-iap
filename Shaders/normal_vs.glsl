#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoords;

out vec3 Normals;
out vec2 TexCoords;
out vec3 FragPos;

const float pi = 3.14159265;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float waveDir;
uniform float wavelength;
uniform float peak;
uniform float time;

void main()
{
    float a = dot(position.x, normalize(waveDir));
	//float a = position.x;
	float b = peak - 1.0f; 
	float k = (2.0f * pi) / wavelength;
	float c = sqrt(9.81f/k);
	
	float wave_dx = (exp(k * b)/ k) * sin(k * (a + c * time));
	float wave_dy = -(exp(k * b)/ k) * cos(k * (a + c * time));
	
	gl_Position = projection * view * model * vec4( (position.x + wave_dx), (position.y + wave_dy), (position.z), 1.0f);
	//gl_Position = projection * view * model *  vec4( (position.x), (position.y), position.z, 1.0f);
    Normals = normals;
	TexCoords = texCoords;
	FragPos = position;
	
}