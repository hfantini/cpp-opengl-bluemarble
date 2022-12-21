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
#include <vector>
#include "camera.hpp"
#include "gtx/string_cast.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int windowWidth = 0;
int windowHeight = 0;
GLint GLMajorVersion = 0;
GLint GLMinorVersion = 0;
glm::vec2 mouseSensitivity{ 0.03f, 0.03f };
glm::vec2 deltaCursorMovement{ 0.0f, 0.0f };
glm::vec2 deltaUpCursorMovement{ 0.0f, 0.0f };
bool enableVerticalMouseMovement = false;
glm::vec2 onClickMousePosition{ 0.0f, 0.0f };
glm::vec3 cameraAnchorPosition{ 0.0f };
Camera* camera;
std::string title = std::string("BLUE MARBLE - HF");

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 UV;
};

struct DirectionalLight
{
	glm::vec3 direction;
	GLfloat intensity;
};

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

GLuint loadTexture(const char* textureFile)
{
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

void createSphereMesh(GLuint resolution, std::vector<Vertex>& vertices, std::vector<glm::ivec3>& indexes)
{
	vertices.clear();
	indexes.clear();

	constexpr float PI = glm::pi<float>();
	constexpr float TWO_PI = glm::two_pi<float>();
	float negativeRes = 1.0f / static_cast<float>(resolution - 1);

	for (GLuint UIndex = 0; UIndex < resolution; ++UIndex)
	{
		const float U = UIndex * negativeRes;
		const float theta = glm::mix(0.0f, TWO_PI, U);

		for (GLuint VIndex = 0; VIndex < resolution; ++VIndex)
		{
			const float V = VIndex * negativeRes;
			const float phi = glm::mix(0.0f, PI, V);

			glm::vec3 vertexPosition =
			{
				glm::cos(theta) * glm::sin(phi),
				glm::sin(theta) * glm::sin(phi),
				glm::cos(phi)
			};

			vertices.push_back(Vertex
				{
				vertexPosition,
				glm::normalize(vertexPosition),
				glm::vec3{ 1.0f, 1.0f, 1.0f },
				glm::vec2{ 1.0f - U, 1.0f - V }
			});
		}
	}

	for (GLuint U = 0; U < resolution - 1; ++U)
	{
		for (GLuint V = 0; V < resolution - 1; ++V)
		{
			GLuint P0 = U + V * resolution;
			GLuint P1 = U + 1 + V * resolution;
			GLuint P2 = U + (V + 1) * resolution;
			GLuint P3 = U + 1 + (V + 1) * resolution;

			indexes.push_back(glm::vec3{ P3, P2, P0 });
			indexes.push_back(glm::vec3{ P1, P3, P0 });
		}
	}
}

GLuint loadSphere(GLuint& numVertices, GLuint& numIndexes)
{
	std::vector<Vertex> vertices;
	std::vector<glm::ivec3> indexes;
	createSphereMesh(100, vertices, indexes);

	numVertices = vertices.size() * sizeof(Vertex);
	numIndexes = indexes.size() * sizeof(glm::ivec3);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndexes, indexes.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

void updateWindowFPS(GLFWwindow* window, const char* original, double fps)
{
	std::stringstream ss;
	ss << original << " | FPS: " << fps;
	glfwSetWindowTitle(window, ss.str().c_str());
}

void resetMouseValues(GLFWwindow* window)
{
	deltaCursorMovement = glm::vec2{ 0.0f };
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			onClickMousePosition = glm::vec2(x, y);
			cameraAnchorPosition = camera->position;
			enableVerticalMouseMovement = true;
		}
		else
		{
			deltaUpCursorMovement = glm::vec2{ 0.0f };
			enableVerticalMouseMovement = false;
		}
	}
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
	glm::vec2 currentCursorPos = { x, y };

	if (enableVerticalMouseMovement)
	{
		deltaUpCursorMovement = currentCursorPos - onClickMousePosition;
	}
	else
	{
		deltaCursorMovement = currentCursorPos - glm::vec2{ windowWidth / 2, windowHeight / 2 };
	}
}

void windowResize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowWidth = height;

	camera->aspectRatio = static_cast<float>(width) / height;
	glViewport(0, 0, width, height);
}

int main()
{
	int retValue = 0;

	try
	{
		if (glfwInit() == GLFW_FALSE)
		{
			MessageBox(NULL, "Failed at init GLFW environment...", "Ops!", MB_OK | MB_ICONERROR);
			glfwTerminate();
			return 1;
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		windowWidth = mode->width;
		windowHeight = mode->height;
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), monitor, NULL);

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
			MessageBox(NULL, message.c_str(), "Ops!", MB_OK | MB_ICONERROR);
			glfwTerminate();
			return 1;
		}

		glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);

		std::stringstream ss;
		ss << title << " (OPENGL " << GLMajorVersion << "." << GLMinorVersion << ")";
		title = ss.str();

		glViewport(0, 0, windowWidth, windowHeight);
		camera = new Camera(windowWidth, windowHeight);

		glfwSetWindowTitle(window, title.c_str());
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		resetMouseValues(window);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetCursorPosCallback(window, mouseMoveCallback);
		glfwSetFramebufferSizeCallback(window, windowResize);

		// LOAD, COMPILE AND LINK PROGRAM BASED ON VERTEX AND FRAGMENT SHADER
		GLint shaderProgramID = loadShader("shaders/triangle_vertex.glsl", "shaders/triangle_frag.glsl");

		// LOAD TEXTURE
		GLuint earthTextureID = loadTexture("textures/earth.jpg");
		GLuint cloudsTextureID = loadTexture("textures/clouds.jpg");

		// LOADING MODEL
		GLuint numVertices = 0;
		GLuint numIndexes = 0;
		GLuint sphereVAO = loadSphere(numVertices, numIndexes);

		// MODEL MATRIX (ROTATE TO FOCUS BRAZIL <3)
		glm::mat4 modelMatrix = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f))
								* glm::rotate(glm::identity<glm::mat4>(), glm::radians(220.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		float spinVelocity = 2.0f;

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		double previousTime = glfwGetTime();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_BUFFER);
		glDepthFunc(GL_LESS);

		DirectionalLight light
		{
			glm::vec3(0.0f, 0.0f, -1.0),
			1.0f
		};

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
			updateWindowFPS(window, title.c_str(), fps);

			// MAKING THE SPHERE ROTATE
			modelMatrix *= glm::rotate(glm::identity<glm::mat4>(), glm::radians<float>(spinVelocity * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));

			// STARTING WITH OPENGL OPERATIONS

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shaderProgramID);

			GLint modelViewProjectionLoc = glGetUniformLocation(shaderProgramID, "modelViewProjection");
			glUniformMatrix4fv(modelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(camera->getViewProjection() * modelMatrix));

			glm::mat4 normalMatrix = glm::inverse(glm::transpose(camera->getView() * modelMatrix));
			GLuint normalMatrixLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");
			glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			// TEXTURES 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, earthTextureID);
			GLint earthTextureLoc = glGetUniformLocation(shaderProgramID, "earthTexture");
			glUniform1i(earthTextureLoc, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cloudsTextureID);
			GLint cloudsTextureLoc = glGetUniformLocation(shaderProgramID, "cloudsTexture");
			glUniform1i(cloudsTextureLoc, 1);

			// LIGHT

			GLint lightDirectionLoc = glGetUniformLocation(shaderProgramID, "lightDirection");
			glUniform3fv(lightDirectionLoc, 1, glm::value_ptr(camera->getView() * glm::vec4{ light.direction, 0.0f }));

			GLint lightIntensityLoc = glGetUniformLocation(shaderProgramID, "lightIntensity");
			glUniform1f(lightIntensityLoc, light.intensity);

			// MISC
			GLint timeLoc = glGetUniformLocation(shaderProgramID, "time");
			glUniform1f(timeLoc, currentTime);
			
			// CHECK FOR KEYBOARD INTERRUPTS

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				camera->moveFoward(2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				camera->moveFoward(-2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				camera->moveRight(2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				camera->moveRight(-2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				camera->roll(-100.0f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				camera->roll(100.0f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			{
				camera->moveUp(2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
			{
				spinVelocity = 500.0f;
			}
			else
			{
				spinVelocity = 2.0f;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			{
				camera->moveUp(-2.5f * deltaTime);
			}

			if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
			{
				resetMouseValues(window);
				camera->reset();
			}

			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				break;
			}

			// CHECK FOR MOUSE MOVEMENT

			if (enableVerticalMouseMovement)
			{
				camera->moveMouseAxis((deltaUpCursorMovement * mouseSensitivity), &cameraAnchorPosition);
			}
			else
			{
				camera->look((deltaCursorMovement * mouseSensitivity) * glm::vec2(-1));
			}

			// DRAW

			glBindVertexArray(sphereVAO);
			glLineWidth(10.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);

			glUseProgram(0);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
	}
	catch (std::exception& e)
	{
		std::stringstream ss;
		ss << "Fatal Exception! The program cannot continue: " << e.what();
		MessageBox(NULL, ss.str().c_str(), "Ops!", MB_OK | MB_ICONERROR);

		retValue = 1;
	}

	// FREE MEMORY REFERENCES

	if (camera)
	{
		delete camera;
	}

	glfwTerminate();

	return retValue;
}