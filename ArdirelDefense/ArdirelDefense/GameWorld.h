#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "GameObject.h"
#include "ShaderManager.h"
using namespace std;
using namespace glm;

class GameWorld
{
public:
	static bool init(GLFWwindow* windowPtr);
	static void update();
	static void draw();
	static void checkVis();
	static void combat(GameObject* player, GameObject* enemy);
	static void checkMove();
	static int randomize(int min, int max);

private:
	static void makePC();
	static void setHealth(GameObject* cPtr, GameObject* iPtr);
	static void setLevel(int lvl, GameObject* ptr);
	static void setPStats(int a, int mc, int mt);
	static void makeBG();
	static void makeEn(int level);
	static void popEn();
	static void charSel();
	static void enVis(GameObject* tile);
	static int findEnLoc();
	static void makeDisplay();
	static char* loadTextFile(const char* file);

	static float getFrameTime();
	static float activeTime;
	static GLuint shaderProgramIndex;
	static GLFWwindow* window;

	static vector<GameObject*> gameObjectPtrCollection;
	static GameObject* playerPtr;

	GameWorld(void);
	~GameWorld(void);
};
