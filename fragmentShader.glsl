#version 420 core

struct Material
{
	vec4 ambiReflect;
	vec4 diffReflect;
	vec4 specReflect;
	vec4 emitColours;
	float shininess;
};
struct Light
{
	vec4 ambiColours;
	vec4 diffColours;
	vec4 specColours;
	vec4 position;
};
uniform Light mainLight;

uniform Material terrainFAndB;
uniform vec4 globalAmbientLight;

in vec3 normal;

out vec4 colorsOut;

void main(void)
{
   vec3 lightDirection  =  normalize(vec3(mainLight.position));
   colorsOut = max(dot(normal, lightDirection), 0.0f) * (mainLight.diffColours * terrainFAndB.diffReflect);
}