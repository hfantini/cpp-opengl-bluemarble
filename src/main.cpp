#include <iostream>
#include <Windows.h>
#include <glew.h>
#include <glfw3.h>
#include <string>
#include <sstream>
#include <array>
#include <glm.hpp>
#include <ext.hpp>
#include <fstream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
GLint GLMajorVersion = 0;
GLint GLMinorVersion = 0;
std::string title = std::string("BLUE MARBLE - HF");

std::string readFile(const char* path)
{
	std::string retValue;
	std::ifstream fileStream;

	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fileStream.open(path, std::ios::in);
	retValue.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

	return retValue;
}

void checkShaderCompileStatus(GLint shaderID)
{
	GLint result = GL_TRUE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint errorLogLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &errorLogLength);

		if (errorLogLength > 0)
		{
			std::string shaderErrorLog(errorLogLength, '\0');
			glGetShaderInfoLog(shaderID, errorLogLength, nullptr, &shaderErrorLog[0]);

			std::stringstream ss;
			ss << "Error at compile shader of id: " << shaderID << ": " << shaderErrorLog;
			throw std::exception(ss.str().c_str());
		}
		else
		{
			throw std::exception("Generic error at compile shader.");
		}
	}
}

void checkProgramCompileStatus(GLint programID)
{
	GLint result = GL_TRUE;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint errorLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &errorLogLength);

		if (errorLogLength > 0)
		{
			std::string programErrorLog(errorLogLength, '\0');
			glGetProgramInfoLog(programID, errorLogLength, nullptr, &programErrorLog[0]);

			std::stringstream ss;
			ss << "Error at link the program of id: " << programID << ": " << programErrorLog;
			throw std::exception(ss.str().c_str());
		}
		else
		{
			throw std::exception("Generic error at link the program");
		}
	}
}

GLuint loadShader(const char* vertexShaderFile, const char* fragmentShaderFile)
{
	std::string vertexShaderSource = readFile(vertexShaderFile);
	std::string fragmentShaderSource = readFile(fragmentShaderFile);

	if (vertexShaderSource.empty())
	{
		throw std::exception("Empty vertex shader source.");
	}

	if (fragmentShaderSource.empty())
	{
		throw std::exception("Empty fragment shader source.");
	}

	// CREATING SHADERS IDENTIFIERS

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSourcePtr = vertexShaderSource.c_str();
	glShaderSource(vertexShaderID, 1, &vertexShaderSourcePtr, nullptr);
	glCompileShader(vertexShaderID);

	checkShaderCompileStatus(vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSourcePtr, nullptr);
	glCompileShader(fragmentShaderID);

	checkShaderCompileStatus(fragmentShaderID);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	checkProgramCompileStatus(programID);

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

int main()
{
	try
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
			std::string message = std::string("Failed at init GLEW: ").append((char*)glewGetErrorString(status));
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

		// LOAD, COMPILE AND LINK PROGRAM BASED ON VERTEX AND FRAGMENT SHADER
		GLint shaderProgramID = loadShader("shaders/triangle_vertex.glsl", "shaders/triangle_frag.glsl");

		// TRIANGLE VERTEX

		std::array<Vertex, 3> triangle =
		{
			Vertex{glm::vec3 {-1.0f, -1.0f, 0.0f}, glm::vec3 {1.0f, 0.0f, 0.0f}},
			Vertex{glm::vec3 {1.0f, -1.0f, 0.0f}, glm::vec3 {0.0f, 1.0f, 0.0f}},
			Vertex{glm::vec3 {0.0f, 1.0f, 0.0f}, glm::vec3 {0.0f, 0.0f, 1.0f}}
		};

		// MODEL MATRIX

		glm::mat4 modelMatrix = glm::identity<glm::mat4>();

		// VIEW MATRIX

		glm::vec3 eye{ 0.0f, 0.0f, -5.0f };
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

		for (Vertex& vertex : triangle)
		{
			glm::vec4 projectedVertex = modelViewProjectionMatrix * glm::vec4{ vertex.position, 1.0f };
			projectedVertex = projectedVertex / projectedVertex.w;
			vertex.position = projectedVertex;
		}


		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle.data(), GL_STATIC_DRAW);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(shaderProgramID);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glUseProgram(0);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		glDeleteBuffers(1, &vertexBuffer);
		glfwTerminate();
		return 0;
	}
	catch (std::exception& e)
	{
		std::stringstream ss;
		ss << "Fatal Exception! The program cannot continue: " << e.what();
		MessageBox(NULL, ss.str().c_str(), "Ops!", MB_OK | MB_ICONERROR);
		return 1;
	}
}