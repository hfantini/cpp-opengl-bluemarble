#include<iostream>
#include<Windows.h>
#include<glfw3.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main()
{
	if (glfwInit() == GLFW_FALSE)
	{
		MessageBoxW(NULL, L"Failed at init GLFW environment...", L"Ops!", MB_OK | MB_ICONERROR);
		glfwTerminate();
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BLUE MARBLE - HF", nullptr, nullptr);
	if (window == nullptr)
	{
		MessageBoxW(NULL, L"Failed at create GLFW Window...", L"Ops!", MB_OK | MB_ICONERROR);
		glfwTerminate();
		return 1;
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}