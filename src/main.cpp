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
#include <math.h>
#include "camera.hpp"
#include "gtx/string_cast.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
GLint GLMajorVersion = 0;
GLint GLMinorVersion = 0;
bool mouseMoveEnabled = false;
glm::vec2 mouseSensitivity{ 0.3f, 0.3f };
glm::vec2 previousCursorPosition{ 0.0f, 0.0f };
glm::vec2 deltaCursorMovement{ 0.0f, 0.0f };
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
	glm::vec2 UV;
};

GLuint loadTexture(const char* textureFile)
{

	stbi_set_flip_vertically_on_load(true);

	int textureWidth = 0;
	int textureHeight = 0;
	int numberOfComponents = 0;

	unsigned char* textureData = stbi_load(textureFile, &textureWidth, &textureHeight, &numberOfComponents, 3);

	if (textureData == nullptr)
	{
		std::stringstream ss;
		ss << "Cannot open texture file: " << textureFile;
		throw std::exception(ss.str().c_str());
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(textureData);

	return textureID;
}

void updateWindowFPS(GLFWwindow* window, const char* original, double fps)
{
	std::stringstream ss;
	ss << original << " | FPS: " << fps;
	glfwSetWindowTitle(window, ss.str().c_str());
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int modifier)
{
	if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			previousCursorPosition = { x, y };
			mouseMoveEnabled = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		
		if (action == GLFW_RELEASE)
		{
			mouseMoveEnabled = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

	}
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
	if (mouseMoveEnabled)
	{
		glm::vec2 currentCursorPos = { x, y };
		deltaCursorMovement = currentCursorPos - previousCursorPosition;
	}
}

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
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetCursorPosCallback(window, mouseMoveCallback);

		// LOAD, COMPILE AND LINK PROGRAM BASED ON VERTEX AND FRAGMENT SHADER
		GLint shaderProgramID = loadShader("shaders/triangle_vertex.glsl", "shaders/triangle_frag.glsl");

		// LOAD TEXTURE
		GLuint textureID = loadTexture("textures/earth.jpg");

		// TRIANGLE VERTEX

		std::array<Vertex, 4> quad =
		{
			Vertex{glm::vec3 {-1.0f, -1.0f, 0.0f}, glm::vec3 {1.0f, 0.0f, 0.0f}, glm::vec2 {0.0, 0.0}},
			Vertex{glm::vec3 {1.0f, -1.0f, 0.0f}, glm::vec3 {0.0f, 1.0f, 0.0f}, glm::vec2 {1.0, 0.0}},
			Vertex{glm::vec3 {1.0f, 1.0f, 0.0f}, glm::vec3 {0.0f, 0.0f, 1.0f}, glm::vec2 {1.0, 1.0}},
			Vertex{glm::vec3 {-1.0f, 1.0f, 0.0f}, glm::vec3 {1.0f, 0.0f, 0.0f}, glm::vec2 {0.0, 1.0}}
		};

		std::array<glm::ivec3, 2> indices =
		{
			glm::ivec3{0, 1, 3},
			glm::ivec3{3, 1, 2}
		};

		// CAMERA

		Camera camera = Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

		// MODEL MATRIX

		glm::mat4 modelMatrix = glm::identity<glm::mat4>();

		// CREATING VEXTEX BUFFER

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);

		GLuint elementBuffer;
		glGenBuffers(1, &elementBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		double previousTime = glfwGetTime();

		while (!glfwWindowShouldClose(window))
		{
			double currentTime = glfwGetTime();
			double deltaTime = currentTime - previousTime;

			if (deltaTime > 0.0)
			{
				previousTime = currentTime;
			}

			// UPDATE TIMINGS

			double fps = std::round( (1000.0 / deltaTime) / 1000.0 );
			//std::cout << "Previous Time: " << previousTime << " | CurrentTime: " << currentTime << " | Delta Time: " << deltaTime << " | FPS: " << fps << std::endl;
			updateWindowFPS(window, title.c_str(), fps);

			// STARTING WITH OPENGL OPERATIONS

			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(shaderProgramID);

			GLint modelViewProjectionLoc = glGetUniformLocation(shaderProgramID, "modelViewProjection");
			glUniformMatrix4fv(modelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewProjection() * modelMatrix));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			GLint textureSamplerLoc = glGetUniformLocation(shaderProgramID, "textureSampler");
			glUniform1i(textureSamplerLoc, 0);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

			glDrawElements(GL_TRIANGLES, sizeof(indices) * 3, GL_UNSIGNED_INT, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glUseProgram(0);

			glfwPollEvents();
			glfwSwapBuffers(window);

			// CHECK FOR KEYBOARD INTERRUPTS

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				camera.moveFoward(2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				camera.moveFoward(-2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				camera.moveRight(2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				camera.moveRight(-2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				camera.roll(-100.0f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				camera.roll(100.0f * deltaTime);
			}

			// CHECK FOR MOUSE MOVEMENT

			if (mouseMoveEnabled)
			{
				camera.look((deltaCursorMovement * mouseSensitivity) * glm::vec2(deltaTime) * glm::vec2(-1));
			}
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
		glfwTerminate();
		return 1;
	}
}