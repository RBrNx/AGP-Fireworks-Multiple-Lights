#ifndef RT3D
#define RT3D

#include <GL/glew.h>
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm\glm.hpp>

#define RT3D_VERTEX		0
#define RT3D_COLOUR		1
#define RT3D_NORMAL		2
#define RT3D_TEXCOORD   3
#define RT3D_INDEX		4

namespace rt3d {

	//struct for toon light
	struct lightStruct {
		GLfloat ambient[4];
		GLfloat diffuse[4];
		GLfloat specular[4];
		GLfloat position[4];
	};

	//struct for point lights
	struct pointLightStruct {
		GLfloat ambient[3];
		GLfloat diffuse[3];
		GLfloat specular[3];
		GLfloat position[3];
		GLfloat constant;
		GLfloat linear;
		GLfloat quadratic;
	};

	//struct for directional lights
	struct dirLightStruct {
		GLfloat ambient[3];
		GLfloat diffuse[3];
		GLfloat specular[3];
		GLfloat direction[3];
	};

	//struct for materials in toon
	struct materialStruct {
		GLfloat ambient[4];
		GLfloat diffuse[4];
		GLfloat specular[4];
		GLfloat shininess;
	};

	/*
	struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	};
	*/

	//struct for materials in multiple light
	struct materialLightStruct {
		GLfloat ambient[3];
		GLfloat diffuse[3];
		GLfloat specular[3];
		GLfloat shininess;
	};


	void exitFatalError(const char *message);
	char* loadFile(const char *fname, GLint &fSize);
	void printShaderError(const GLint shader);
	GLuint initShaders(const char *vertFile, const char *fragFile);

	// Some methods for creating meshes
	// ... including one for dealing with indexed meshes
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, const GLfloat* normals,
		const GLfloat* texcoords, const GLuint indexCount, const GLuint* indices);

	// these three create mesh functions simply provide more basic access to the full version
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, const GLfloat* normals,
		const GLfloat* texcoords);
	GLuint createMesh(const GLuint numVerts, const GLfloat* vertices);
	GLuint createColourMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours);

	void setUniformMatrix4fv(const GLuint program, const char* uniformName, const GLfloat *data);
	void setMatrices(const GLuint program, const GLfloat *proj, const GLfloat *mv, const GLfloat *mvp);
	
	//sets data for toon light
	void setLight(const GLuint program, const lightStruct light);

	//sets data for point lights
	void setPointLight(const GLuint program, const pointLightStruct light, GLint pointIndex);
	//void setPointLight2(const GLuint program, const pointLightStruct light);
	//void setPointLight3(const GLuint program, const pointLightStruct light);

	//sets data for directional light
	void setDirLight(const GLuint program, const dirLightStruct light);

	void setLightPos(const GLuint program, const GLfloat *lightPos);

	void setPointLightPos(const GLuint program, const GLfloat *lightPos);

	void setPointLightPosition(const GLuint program, const pointLightStruct light, GLint pointIndex);

	//sets material data for toon and environment map shaders
	void setMaterial(const GLuint program, const materialStruct material);

	//sets data for multiple light shader
	void setLightMaterial(const GLuint program, const materialLightStruct material);

	void drawMesh(const GLuint mesh, const GLuint numVerts, const GLuint primitive); 
	void drawIndexedMesh(const GLuint mesh, const GLuint indexCount, const GLuint primitive);

	void updateMesh(const GLuint mesh, const unsigned int bufferType, const GLfloat *data, const GLuint size);
}

#endif