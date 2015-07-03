#pragma once
#include "GameWorld.h"
#include <glew.h>
#include <glfw3.h>

class WindowManager
{
public:
	static int init();
	static GLFWwindow* windowPtr;

private:
	WindowManager(void);
	~WindowManager(void);
};

