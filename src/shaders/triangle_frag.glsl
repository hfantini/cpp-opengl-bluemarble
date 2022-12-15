#version 330 core

uniform sampler2D textureSampler;

in vec3 color; 
in vec2 UV;

out vec4 outColor;

void main()
{
	vec3 textureColor = texture(textureSampler, UV).rgb;
	outColor = vec4(textureColor, 1.0);
}