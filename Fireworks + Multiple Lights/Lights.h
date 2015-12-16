#pragma once
#include "rt3d.h"
#include <vector>
#include <glm/glm.hpp>

class Lights
{
private:	
	// light attenuation
	float attConstant = 1.0f;
	float attLinear = 0.0f;
	float attQuadratic = 0.00f;

	rt3d::pointLightStruct pointLightArray[20];

	float randomR;
	float randomG;
	float randomB;

	float randomX;
	float randomZ;

	int deletedLights = 0;

public:

	Lights();
	void init();
	void update(int i, float explodeTime);
	void draw();
	void addLight(glm::vec3 colour, glm::vec3 position, int i);
	void deleteLight() { deletedLights += 1; }

	GLuint multipleLightShader;
	std::vector<rt3d::pointLightStruct> lightVector;

	float returnConstant() { return attConstant;  }
	float returnLinear() { return attLinear; }
	float returnQuadratic() { return attQuadratic;  }

	glm::vec3 returnPosition(int spaceCounter);
	glm::vec3 returnColour(int spaceCounter);

	int returnDeletedLights() { return deletedLights; }

	~Lights();
};

