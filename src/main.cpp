#include <iostream>
#include <Windows.h>
#include <glew.h>
#include <glfw3.h>
#include <string>
#include <sstream>
#include <array>
#include <glm.hpp>
#include <ext.hpp>

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

	// TRIANGLE VERTEX

	std::array<glm::vec3, 3> triangle =
	{
		glm::vec3 {-1.0f, -1.0f, 0.0f},
		glm::vec3 {1.0f, -1.0f, 0.0f},
		glm::vec3 {0.0f, 1.0f, 0.0f}
	};

	// MODEL MATRIX

	glm::mat4 modelMatrix = glm::identity<glm::mat4>();

	// VIEW MATRIX

	glm::vec3 eye{ 0.0f, 0.0f, -10.0f };
	glm::vec3 center{ 0.0f, 0.0f, 00.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	glm::mat4 viewMatrix = glm::lookAt(eye, center, up);

	// PROJECTION MATRIX

	constexpr float fov = glm::radians(45.0f);
	const float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
	const float fNear = 0.001f;
	const float fFar = 1000.0f;

	glm::mat4 projectionMatrix = glm::perspective(fov, aspectRatio, fNear, fFar);

	// MODEL VIEW PROJECTION (MADE BY THE CPU FOR TESTING PURPOSES)

	glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

	// APPLYING THE modelViewProjectionMatrix IN EACH TRIANGLE VERTEX

	for (glm::vec3& vertex : triangle)
	{
		glm::vec4 projectedVertex = modelViewProjectionMatrix * glm::vec4{ vertex, 1.0f };
		projectedVertex = projectedVertex / projectedVertex.w;
		vertex = projectedVertex;
	}


	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle.data(), GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &vertexBuffer);

	glfwTerminate();
	return 0;
}