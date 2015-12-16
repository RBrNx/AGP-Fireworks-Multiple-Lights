#include "main.h"

// Globals
// Real programs don't use globals :-D

GLuint meshIndexCount = 0; //Number of indexes in each mesh
GLuint meshObjects[3]; //array of mesh objects

GLuint skyboxProgram; //skybox shaders
GLuint particleProgram; //particle system shaders
GLuint bumpMapProgram; //bump mapping shaders

//3 Vec3's used for the Camera
glm::vec3 eye(-9.9f, 13.3f, 37.0f);
glm::vec3 at(0.0f, 1.0f, -1.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
float r = 0.0f; //Used for camera rotation

stack<glm::mat4> mvStack; 

//Vector for the particle system
vector<particleArray*> particleVector;

GLuint textures[7]; //Array for textures
GLuint skybox[5]; //Array for skybox Textures

//Structs
//Struct for directional light
rt3d::dirLightStruct directionalLight = {
		{ 0.2f, 0.2f, 0.2f }, // ambient
		{ 0.4f, 0.4f, 0.4f }, // diffuse
		{ 0.5f, 0.5f, 0.5f }, // specular
		{ 0.0f, 0.0f, 0.0f }  // direction
};

//Light properties for the walls
rt3d::lightStruct light0 = {
		{ 0.4f, 0.4f, 0.4f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ -5.0f, 2.0f, 2.0f, 1.0f }  // position
};

//Material properties for Ground
rt3d::materialLightStruct lightMaterial = {
		{ 0.4, 0.4f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f }, // specular
		1.0f  // shininess
};

//Material properties for the walls
rt3d::materialStruct material0 = {
		{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
		{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		1.0f  // shininess
};

rt3d::pointLightStruct emptyStruct = {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		1.0f,
		1.0f,
		0.132f
};

bool spaceDown; //bool to check if Space is pressed
bool nDown; //bool to check if N is pressed
bool drawParticle; //bool to add 1 firework to the scene
bool addLight; //bool to add 1 light to the scene
GLint boolLights; //Bool to turn lights on and off
int spaceCounter; //counts how many times space has been pressed

//Variables used for the timer
int currentTime;
int differentTime;
int lastTime;
int counter;

//Arrays for storing the positions and colours to be used for the lights and particles
glm::vec3 positions[20];
glm::vec3 colours[20];

Lights *lightSystem; // init light system

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	// Create 1920 x 1080 window
	//window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	//	1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1600, 900, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

// A simple texture loading function
// lots of room for improvement - and better error checking!
GLuint loadBitmap(char *fname) {
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

	// load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}


// A simple cubemap loading function
// lots of room for improvement - and better error checking!
GLuint loadCubeMap(const char *fname[6], GLuint *texID)
{
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0; i<6; i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}

void calculateTangents(vector<GLfloat> &tangents, vector<GLfloat> &verts, vector<GLfloat> &normals, vector<GLfloat> &tex_coords, vector<GLuint> &indices){

	// Code taken from http://www.terathon.com/code/tangent.html and modified slightly to use vectors instead of arrays
	// Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. 

	// This is a little messy because my vectors are of type GLfloat:
	// should have made them glm::vec2 and glm::vec3 - life, would be much easier!

	vector<glm::vec3> tan1(verts.size() / 3, glm::vec3(0.0f));
	vector<glm::vec3> tan2(verts.size() / 3, glm::vec3(0.0f));
	int triCount = indices.size() / 3;
	for (int c = 0; c < indices.size(); c += 3)
	{
		int i1 = indices[c];
		int i2 = indices[c + 1];
		int i3 = indices[c + 2];

		glm::vec3 v1(verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]);
		glm::vec3 v2(verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]);
		glm::vec3 v3(verts[i3 * 3], verts[i3 * 3 + 1], verts[i3 * 3 + 2]);

		glm::vec2 w1(tex_coords[i1 * 2], tex_coords[i1 * 2 + 1]);
		glm::vec2 w2(tex_coords[i2 * 2], tex_coords[i2 * 2 + 1]);
		glm::vec2 w3(tex_coords[i3 * 2], tex_coords[i3 * 2 + 1]);

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (int a = 0; a < verts.size(); a += 3)
	{
		glm::vec3 n(normals[a], normals[a + 1], normals[a + 2]);
		glm::vec3 t = tan1[a / 3];

		glm::vec3 tangent;
		tangent = (t - n * glm::normalize(glm::dot(n, t)));

		// handedness
		GLfloat w = (glm::dot(glm::cross(n, t), tan2[a / 3]) < 0.0f) ? -1.0f : 1.0f;

		tangents.push_back(tangent.x);
		tangents.push_back(tangent.y);
		tangents.push_back(tangent.z);
		tangents.push_back(w);

	}
}

void randomiseArray(){
	//This function fills two arrays. One with Random positions and one with Random Colours
	for (int i = 0; i < 20; i++){
		glm::vec3 previousPosition;
		float randomX = -30 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (40 - 5));
		float randomZ = -30 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (40 - 5));
		glm::vec3 randomPosition = glm::vec3(randomX, 0, randomZ); //Make a random Position
		if (randomPosition == previousPosition){ //Check to see if it already exists and re-random
			float randomX = -30 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (40 - 5));
			float randomZ = -30 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (40 - 5));
			randomPosition = glm::vec3(randomX, 0, randomZ);
			previousPosition = randomPosition;

		}

		//Make a random colour
		float randomR = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float randomG = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float randomB = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		positions[i] = randomPosition;
		colours[i] = glm::vec3(randomR, randomG, randomB);

	}
}

void init() {
	lightSystem = new Lights(); //Initalise Lights

	rt3d::setDirLight(lightSystem->multipleLightShader, directionalLight); //Send directional light struct to the Shaders

	GLint viewPosLoc = glGetUniformLocation(lightSystem->multipleLightShader, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(eye)); //Send the viewpos of the camera to the shaders
	
	skyboxProgram = rt3d::initShaders("Shaders/cubeMap.vert", "Shaders/cubeMap.frag"); //Load Skybox Shaders
	particleProgram = rt3d::initShaders("Shaders/particles.vert", "Shaders/particles.frag"); //Load Particle System shaders
	bumpMapProgram = rt3d::initShaders("Shaders/normalmap.vert", "Shaders/normalmap.frag"); //Load Normal/Bump Map shaders

	//Load Skybox textures into the array
	const char *cubeTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};
	loadCubeMap(cubeTexFiles, &skybox[0]);

	//Load the cube.obj
	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	//Calculate tangents for the bump mapping
	vector<GLfloat> tangents;
	calculateTangents(tangents, verts, norms, tex_coords, indices);
	
	//Create the cube mesh
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());

	glBindVertexArray(meshObjects[0]);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	// VBO for tangent data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(GLfloat), tangents.data(), GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(5);
	glBindVertexArray(0);

	//Load all the textures here
	textures[0] = loadBitmap("Textures/Grass.bmp");
	textures[1] = loadBitmap("Textures/studdedmetal.bmp");
	textures[2] = loadBitmap("Textures/Brick.bmp");
	textures[3] = loadBitmap("Textures/fieldstone-c.bmp");
	textures[4] = loadBitmap("Textures/smoke1.bmp");
	textures[5] = loadBitmap("Textures/paper.bmp");
	textures[6] = loadBitmap("Textures/fieldstone-n.bmp");

	//Pass lighting constants to shaders
	glUseProgram(bumpMapProgram);
	rt3d::setLight(bumpMapProgram, light0);
	rt3d::setMaterial(bumpMapProgram, material0);
	int uniformIndex = glGetUniformLocation(bumpMapProgram, "attConst");
	glUniform1f(uniformIndex, lightSystem->returnConstant());
	uniformIndex = glGetUniformLocation(bumpMapProgram, "attLinear");
	glUniform1f(uniformIndex, lightSystem->returnLinear());
	uniformIndex = glGetUniformLocation(bumpMapProgram, "attQuadratic");
	glUniform1f(uniformIndex, lightSystem->returnQuadratic());

	glUseProgram(lightSystem->multipleLightShader);
    uniformIndex = glGetUniformLocation(lightSystem->multipleLightShader, "attConst");
	glUniform1f(uniformIndex, lightSystem->returnConstant());
	uniformIndex = glGetUniformLocation(lightSystem->multipleLightShader, "attLinear");
	glUniform1f(uniformIndex, lightSystem->returnLinear());
	uniformIndex = glGetUniformLocation(lightSystem->multipleLightShader, "attQuadratic");
	glUniform1f(uniformIndex, lightSystem->returnQuadratic());

	glEnable(GL_DEPTH_TEST); //Enable depth testing
	glEnable(GL_BLEND); //Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(0)); //Seed the rand() function

	glUseProgram(lightSystem->multipleLightShader);

	randomiseArray(); //Fill the position and colours array

}

//Moves the camera forward and back
glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(r*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(r*DEG_TO_RADIAN));
}

//Moves the camera left and right
glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::cos(r*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(r*DEG_TO_RADIAN));
}

void update(void) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, r, 0.1f); //Camera forward
	if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, r, -0.1f); //Camera back
	if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, r, -0.1f); //Camera left
	if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, r, 0.1f); //Camera right
	if (keys[SDL_SCANCODE_R]) eye.y += 0.1f; //Camera up
	if (keys[SDL_SCANCODE_F]) eye.y -= 0.1f; //Camera down
	if (keys[SDL_SCANCODE_COMMA]) r -= 1.0f; //Rotate camera left
	if (keys[SDL_SCANCODE_PERIOD]) r += 1.0f; //Rotate camera right

	if (keys[SDL_SCANCODE_SPACE]) { //Fire a firework and a light
		drawParticle = true; addLight = true;
	}

	if (keys[SDL_SCANCODE_N]) {
		if (boolLights == 1){
			boolLights = 0;
		}
		else { boolLights = 1; }
	}

	if (keys[SDL_SCANCODE_ESCAPE]) exit(1); //Exit program

	if (keys[SDL_SCANCODE_1]) { //Turn on wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_2]) { //Turn off wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
}

void renderCubes(glm::mat4 projection, glm::mat4 viewMatrix) {
	//This function just draws all the objects in the scene, this makes Draw() a little tidier

	// draw a cube for ground plane
	glUseProgram(lightSystem->multipleLightShader);
	rt3d::setLightMaterial(lightSystem->multipleLightShader, lightMaterial);
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "projection", glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	glm::mat4 modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	// draw a bunch of cube blocks on top of ground plane
	//back left
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-29.5f, 1.0f, -29.5f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.5f, 1.0f, 0.5f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);

	mvStack.pop();

	//front right
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(9.5f, 1.0f, 9.5f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.5f, 1.0f, 0.5f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);

	mvStack.pop();

	//back right
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(9.5f, 1.0f, -29.5f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.5f, 1.0f, 0.5f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);

	mvStack.pop();

	//front left
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-29.5f, 1.0f, 9.5f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.5f, 1.0f, 0.5f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);

	mvStack.pop();
	
	//Creates the 8 floating cubes
	float r = 0;

	for (int i = 0; i <= 8; i++){
		float theta = 2.0f * 3.1415926f * float(i) / float(8);

		float cx = -10;
		float cz = -10;

		float x = 10 * cosf(theta);//calculate the x component 
		float z = 10 * sinf(theta);//calculate the z component

		glUseProgram(lightSystem->multipleLightShader);
		rt3d::setLightMaterial(lightSystem->multipleLightShader, lightMaterial);
		rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "projection", glm::value_ptr(projection));

		glUseProgram(bumpMapProgram);
		//rt3d::setLightMaterial(bumpMapProgram, lightMaterial);
		rt3d::setUniformMatrix4fv(bumpMapProgram, "projection", glm::value_ptr(projection));
		glBindTexture(GL_TEXTURE_2D, textures[3]);

		int uniformIndex = glGetUniformLocation(bumpMapProgram, "normalMap");
		glUniform1i(uniformIndex, 1);
		// set tex sampler to texture unit 0, arbitrarily
		uniformIndex = glGetUniformLocation(bumpMapProgram, "texMap");
		glUniform1i(uniformIndex, 0);
		// Now bind textures to texture units
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[6]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[3]);

		glm::mat4 modelMatrix(1.0);
		mvStack.push(mvStack.top());
		modelMatrix = glm::translate(modelMatrix, glm::vec3((cx + x), 5.0f, (cz + z)));
		modelMatrix = glm::rotate(modelMatrix, r, glm::vec3(0, 1, 0));
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
		mvStack.top() = mvStack.top() * modelMatrix;
		uniformIndex = glGetUniformLocation(bumpMapProgram, "cameraPos");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(eye));
		rt3d::setUniformMatrix4fv(bumpMapProgram, "modelMatrix", glm::value_ptr(modelMatrix));
		rt3d::setUniformMatrix4fv(bumpMapProgram, "modelview", glm::value_ptr(mvStack.top()));
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		mvStack.pop();

		r += 45;
		
	}

	//Back Wall
	glUseProgram(lightSystem->multipleLightShader);
	rt3d::setLightMaterial(lightSystem->multipleLightShader, lightMaterial);
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "projection", glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, 20.0f, -30.1f));
	mvStack.top() = glm::rotate(mvStack.top(), 90.0f, glm::vec3(1, 0, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	//Left Wall
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-30.1f, 20.0f, -10.1f));
	mvStack.top() = glm::rotate(mvStack.top(), 90.0f, glm::vec3(0, 0, 1));
	mvStack.top() = glm::rotate(mvStack.top(), 90.0f, glm::vec3(0, 1, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	//Right Wall
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(10.1f, 20.0f, -10.1f));
	mvStack.top() = glm::rotate(mvStack.top(), 90.0f, glm::vec3(0, 0, 1));
	mvStack.top() = glm::rotate(mvStack.top(), 90.0f, glm::vec3(0, 1, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "modelview", glm::value_ptr(mvStack.top()));
	modelMat = (glm::inverse(viewMatrix)) * mvStack.top();
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "model", glm::value_ptr(modelMat));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	
}


void draw(SDL_Window * window) {
	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(lightSystem->multipleLightShader);

	glm::mat4 projection(1.0);
	projection = glm::perspective(60.0f, 1920.0f / 1080.0f, 0.1f, 150.0f);
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "projection", glm::value_ptr(projection));

	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelMat;
	glm::mat4 modelview(1.0);
	mvStack.push(modelview);

	at = moveForward(eye, r, 1.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, at, up);
	rt3d::setUniformMatrix4fv(lightSystem->multipleLightShader, "view", glm::value_ptr(viewMatrix));

	mvStack.top() = viewMatrix;

	//RENDER THE SKYBOX
	// skybox as single cube using cube map
	glUseProgram(skyboxProgram);
	rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));

	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK); // drawing inside of cube!

	// back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on

	//Draw all the objects in the scene
	renderCubes(projection, viewMatrix);

	//draw particles
	glDepthMask(0);
	glUseProgram(particleProgram);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_COLOR);
	glEnable(GL_BLEND);
	GLint loc = glGetUniformLocation(particleProgram, "particleLifetime");

	//pass modelview and projection to uniforms in the shader
	rt3d::setUniformMatrix4fv(particleProgram, "mvpMat", glm::value_ptr(projection*mvStack.top()));

	//Updates and draws any fireworks  and lights that exist
	if (particleVector.size() != NULL)
	{
		for (int i = 0; i < particleVector.size(); i++)
		{
			glUniform1f(loc, particleVector[i]->getLifetime());

			glUseProgram(lightSystem->multipleLightShader);
			lightSystem->update(i, particleVector[i]->getExplodeTime());
			glUseProgram(particleProgram);
			particleVector[i]->update();
			particleVector[i]->draw();
		}
	}

	//Spacecounter counts how many lights/fireworks exist
	spaceCounter = lightSystem->lightVector.size();

	//Launch a firework and a light
	if (drawParticle == true && addLight == true)
	{
		if (spaceDown == true)
		{
			if (particleVector.size() < 20 && lightSystem->lightVector.size() < 20){
				particleVector.push_back(new particleArray(500, positions[spaceCounter], colours[spaceCounter])); //Creates a firework with 500 particles and chooses a colour and position from the arrays
				glUseProgram(lightSystem->multipleLightShader);
				lightSystem->addLight(colours[spaceCounter], positions[spaceCounter], spaceCounter); //Creates a light and chooses a colour and position same as the firework
				drawParticle = false;
				spaceDown = false;
				cout << spaceCounter << endl;
			}
		}

	}

	//Set up a timer to re-randomise the arrays every 100th of a second
	currentTime = SDL_GetTicks();
	differentTime = currentTime - lastTime;

	counter += differentTime;

	if (counter > 10){
		counter = 0;
		randomiseArray();
	}

	lastTime = currentTime;

	//Erases any fireworks and lights that have finished exploding
	for (int i = 0; i < particleVector.size(); i++)
	{
		if (particleVector[i]->getLifetime() < 0)
		{
			lightSystem->lightVector.erase(lightSystem->lightVector.begin() + i);
			glUseProgram(lightSystem->multipleLightShader);
			particleVector.erase(particleVector.begin() + i);
		}
	}

	//Sends amount of lights that exist to the shader so that it only draws how many should exist
	int uniformIndex = glGetUniformLocation(lightSystem->multipleLightShader, "currentLights");
	glUniform1i(uniformIndex, lightSystem->lightVector.size());

	//Tells the shader wether lights should be on or off
	uniformIndex = glGetUniformLocation(lightSystem->multipleLightShader, "boolLights");
	glUniform1i(uniformIndex, boolLights);

	glDisable(GL_BLEND);

	// remember to use at least one pop operation per push...
	mvStack.pop(); // initial matrix
	glDepthMask(GL_TRUE);


	SDL_GL_SwapWindow(window); // swap buffers,,
}


// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {
	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = setupRC(glContext); // Create window and render context 

	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << glGetString(GL_VERSION) << endl;

	init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running)	{	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			switch (sdlEvent.type){
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_SPACE){
					spaceDown = true;
				}
				if (sdlEvent.key.keysym.sym == SDLK_n){
					nDown = true;
				}
				break;
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		update();
		draw(hWindow); // call the draw function
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}