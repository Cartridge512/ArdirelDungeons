#pragma once
#include <glew.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <iostream>
#include <cmath>
#include <SOIL.h>
#include <glm\glm.hpp>
#include <vector>
using namespace glm;
using namespace std;

class GameObject
{
public:
	GameObject(void);
	GameObject(GLfloat vertices[], GLsizei numVerts, GLuint program);
	virtual ~GameObject(void);

	void draw();
	int tile;
	vec3 position;
	vec3 velocity;
	vec3 objscale;
	vec3 rotationAxis;
	float rotationAngle;
	vec3 color;
	bool visible, alive;
	GLuint texture;
	bool textured;
	void loadTex(char* file);
	char type, spec;
	int health, maxHealth, level, exp;
	int nlCalc();
	int atk, init;
	int hIndex, lIndex;

private:
	
	GLuint vboIndex;
	GLuint vaoIndex;
	GLsizei const numVerts;
	int const floatsPerVert;
	GLsizei const numFloats;

	GLuint program;
	GLint uniformColorLocation, uniformWorldMatrixLocation, uniformTexturedLocation;

};

