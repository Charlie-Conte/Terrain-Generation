#include "Lighting.h"



Lighting::Lighting()
{
}




Material::Material()
{
	ambiReflect = glm::vec4(0);
	diffReflect = glm::vec4(0);
	specReflect = glm::vec4(0);
	emitColours = glm::vec4(0);
	shininess = 0;
}
Material::Material(glm::vec4 aR, glm::vec4 dR, glm::vec4 sR, glm::vec4 eC,float shine)
{
	 ambiReflect = aR;
	 diffReflect = dR;
	 specReflect = sR;
	 emitColours = eC;
	 shininess = shine;
}

Light::Light()
{
	ambiColours = glm::vec4(0);
	diffColours = glm::vec4(0);
	specColours = glm::vec4(1);
	position = glm::vec4(0);
}

Light::Light(glm::vec4 aC, glm::vec4 dC, glm::vec4 sC, glm::vec4 pos)
{
	ambiColours = aC;
	diffColours = dC;
	specColours = sC;
	position = pos;
}
