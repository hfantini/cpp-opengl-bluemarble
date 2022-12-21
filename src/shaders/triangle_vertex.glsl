#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;

uniform mat4 modelViewProjection;
uniform mat4 normalMatrix;

out vec3 normal;
out vec3 color;
out vec2 UV;

void main()
{
	normal = vec3(normalMatrix * vec4(inNormal, 0.0));
	color = inColor;
	UV = inUV;
	gl_Position = modelViewProjection * vec4(inPosition, 1.0);
}