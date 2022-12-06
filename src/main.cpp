#include<iostream>
#include<Windows.h>
#include<glew.h>
#include<glfw3.h>
#include<string>
#include <sstream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
GLint GLMajorVersion = 0;
GLint GLMinorVersion = 0;
std::string title = std::string("BLUE MARBLE - HF");

int main()
{
	if (glfwInit() == GLFW_FALSE)
	{
		MessageBox(NULL, "Failed at init GLFW environment...", "Ops!", MB_OK | MB_ICONERROR);
		glfwTerminate();
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), nullptr, nullptr);
	if (window == nullptr)
	{
		MessageBox(NULL, "Failed at create GLFW Window...", "Ops!", MB_OK | MB_ICONERROR);
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::string message = std::string("Failed at init GLEW: ").append( (char *) glewGetErrorString(status));
;		MessageBox(NULL, message.c_str(), "Ops!", MB_OK | MB_ICONERROR);
		glfwTerminate();
		return 1;
	}

	glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);

	std::stringstream ss;
	ss << title << " (OPENGL " << GLMajorVersion << "." << GLMinorVersion << ")";
	title = ss.str();

	glfwSetWindowTitle(window, title.c_str());

	glClearColor(0.0, 0.0, 1.0, 0.0);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}