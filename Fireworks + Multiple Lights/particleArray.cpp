#include "particleArray.h"
#include <ctime>
#include <iostream>
#include <glm\gtc\type_ptr.hpp>

particleArray::particleArray(const int n, glm::vec3 position, glm::vec3 colour) : numParticles(n)
{

	if (numParticles <= 0) // trap invalid input
		return;

	//Create the arrays for the number of particles 
	positions = new glm::vec3[numParticles];
	colours = new glm::vec3[numParticles];
	vel = new glm::vec3[numParticles];

	gravity = 0.21;

	//Seed the rand() function
	std::srand(std::time(0));

	//Fill the arrays
	for (int i = 0; i < numParticles; i++){
		colours[i] = colour;
		positions[i] = position;
		vel[i] = glm::normalize(glm::vec3((rand() % 1500 - 750) / 10000.0f, (rand() % 1500 - 750) / 10000.0f, (rand() % 1500 - 750) / 10000.0f));//Chooses a random velocity for each firework and then normalizes it to produce a circular explosion
		vel[i] /= glm::vec3(10, 10, 10); //Then divides the normalized vector to make the explosion smaller
	}
	vel2 = glm::vec3(0.2, 0.2, 0.2);

	//set explodeTime to random value between 30 and 90(0.5 - 1.5 seconds til explosion)
	explodeTime = rand() % 30 + 60;
	lifeTime = explodeTime;

	glPointSize(17);

	//Generate VAO
	glGenVertexArrays(1, &VAO[0]);
	glBindVertexArray(VAO[0]);

	//Generate 3 VBOs
	glGenBuffers(3, VBOs);

	//Bind VBO[0] and store the positions array
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat)*3) *numParticles, &positions[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Bind VBO[1] and store the colours array
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * 3) *numParticles, &colours[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//Bind VBO[2] and store the velocitys array
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * 3) *numParticles, &vel[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void particleArray::draw(){
	
	glBindVertexArray(VAO[0]); // bind VAO 0 as current object
	// particle data may have been updated - so need to resend to GPU
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // bind VBO 0
	glBufferSubData(GL_ARRAY_BUFFER, NULL, (3 * sizeof(GLfloat))*numParticles, &positions[0]);

	// Position data in attribute index 0, 3 floats per vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);     // Enable attribute index 0

	// Now draw the particles
	glDrawArrays(GL_POINTS, 0, numParticles );
	glBindVertexArray(0); 
}

void particleArray::update(){
	//Decrease explodeTime each frame
	explodeTime --;

	//Make each particle in the firework fly upwards
	for (int i = 0; i < numParticles; i++){
		positions[i].y += vel2.y;

		//Until the firework has exploded, then make the particles move like an explosion
		if (explodeTime < 0) {
				positions[i] += vel[i];
				positions[i].y -= gravity;
		}
	}

	//Start decreasing lifetime of each particle after it has exploded
 	if (explodeTime < 0) {
		lifeTime--;
	}
	


}

particleArray::~particleArray(void)
{
	delete positions;
	delete colours;
	delete vel;
}
