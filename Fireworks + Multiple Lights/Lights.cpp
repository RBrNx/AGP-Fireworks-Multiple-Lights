#include "Lights.h"


Lights::Lights()
{
	init();
}

void Lights::init(){
	multipleLightShader = rt3d::initShaders("Shaders/multipleLight.vert", "Shaders/multipleLight.frag");
}

void Lights::draw(){


}

void Lights::update(int i, float explodeTime){
	
	if (explodeTime > 0){
		pointLightArray[i].position[1] += 0.2f;
	}
	rt3d::setPointLightPosition(multipleLightShader, pointLightArray[i], i);
}

void Lights::addLight(glm::vec3 colour, glm::vec3 position, int i){

	pointLightArray[i] = {
			{ colour.r, colour.g, colour.b },
			{ colour.r, colour.g, colour.b },
			{ colour.r, colour.g, colour.b },
			{ position.x, position.y, position.z },
			1.0f,
			1.0f,
			0.132f
	};

	lightVector.push_back(pointLightArray[i]);

  	rt3d::setPointLight(multipleLightShader, lightVector.at(i), i);
}

glm::vec3 Lights::returnPosition(int spaceCounter){

	return glm::vec3(pointLightArray[spaceCounter].position[0], 0, pointLightArray[spaceCounter].position[2]);

}

glm::vec3 Lights::returnColour(int spaceCounter){

	return glm::vec3(pointLightArray[spaceCounter].ambient[0], pointLightArray[spaceCounter].ambient[1], pointLightArray[spaceCounter].ambient[2]);

}

Lights::~Lights()
{
}
