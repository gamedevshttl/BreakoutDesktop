#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include "game.h"

const GLuint SCREEN_WIDTH = 1024;
const GLuint SCREEN_HEIGHT = 768;

game breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			breakout.m_key[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			breakout.m_key[key] = GL_FALSE;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	breakout.mouse_callback(xpos, ypos);
}

int main(int argc, char *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	breakout.init();

	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;

	breakout.m_state = game_active;

	while (!glfwWindowShouldClose(window))
	{
		GLfloat current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		glfwPollEvents();

		breakout.progress_input(delta_time);
		breakout.update(delta_time);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		breakout.render();

		glfwSwapBuffers(window);
	}


	glfwTerminate();
	return 0;
}