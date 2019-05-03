#pragma once
#include <glm/glm.hpp>
class Lighting
{
public:
	Lighting();



};

class Material
{
public:
	Material();
	Material(glm::vec4 aR, glm::vec4 dR, glm::vec4 sR, glm::vec4 eC, float shine);


	glm::vec4 ambiReflect;
	glm::vec4 diffReflect;
	glm::vec4 specReflect;
	glm::vec4 emitColours;
	float shininess;
};

class Light
{
public:
	Light();
	Light(glm::vec4 aC, glm::vec4 dC, glm::vec4 sC, glm::vec4 pos);

	glm::vec4 ambiColours;
	glm::vec4 diffColours;
	glm::vec4 specColours;
	glm::vec4 position;
};