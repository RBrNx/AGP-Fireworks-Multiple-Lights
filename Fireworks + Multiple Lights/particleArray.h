#pragma once
#include "rt3d.h"
#include <glm\glm.hpp>
#include "Lights.h"

class particleArray {
private:
	int numParticles; //Number of particles in each firework
	glm::vec3* positions; //array for the positions of each particle
	glm::vec3* colours; //array for the colours of each particle
	glm::vec3* vel; //array for the velocitys of the particles at explosion
	glm::vec3 vel2; //array for the velocitys of the particles when firing

	GLuint VAO[1]; //Array of 1 VAO
	GLuint VBOs[3]; //Array of 3 VBOs

	float lifeTime; //Lifetime of each firework
	float explodeTime; //Time till the firework explodes
	float gravity; //Gravity to stop the fireworks going on forever
	

public:
	particleArray(const int n, glm::vec3 position, glm::vec3 colour);
	~particleArray(); 
	int getNumParticles(void) const { return numParticles; }
	glm::vec3* getPositions(void) const { return positions; }
	glm::vec3* getColours(void) const { return colours; }
	glm::vec3* getVel(void) const { return vel; }
	float getLifetime(void) { return lifeTime; }
	float getExplodeTime(void) { return explodeTime; }

	void update(void);
	void draw();
};

