#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

uniform mat4 modelViewProjection;

out vec3 color;
out vec2 UV;

void main()
{
	color = inColor;
	UV = inUV;
	gl_Position = modelViewProjection * vec4(inPosition, 1.0);
}