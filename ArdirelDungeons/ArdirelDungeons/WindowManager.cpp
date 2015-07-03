#include "WindowManager.h"

GLFWwindow* WindowManager::windowPtr = nullptr;

int WindowManager::init()
{
	//Code source: http://www.glfw.org/documentation.html

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    windowPtr = glfwCreateWindow(1440, 960, "Ardirel Defense", NULL, NULL);
    if (!windowPtr)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(windowPtr);

	/* Initialize the game world */
	if (!GameWorld::init(windowPtr))
	{
		glfwTerminate();
		return -1;
	}

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(windowPtr))
    {
        /* Update and render the Game */
		GameWorld::update();
		GameWorld::draw();

        /* Swap front and back buffers */
        glfwSwapBuffers(windowPtr);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

	return 0;
}
WindowManager::WindowManager(void)
{
}


WindowManager::~WindowManager(void)
{
}