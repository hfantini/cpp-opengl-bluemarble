#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

uniform mat4 modelViewProjection;

out vec3 color;

void main()
{
	color = inColor;
	gl_Position = modelViewProjection * vec4(inPosition, 1.0);
}