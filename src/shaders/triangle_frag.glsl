#version 330 core

uniform sampler2D earthTexture;
uniform sampler2D cloudsTexture;
uniform vec3 lightDirection;
uniform float lightIntensity;
uniform float time;

in vec3 normal;
in vec3 color; 
in vec2 UV;

out vec4 outColor;

void main()
{
	vec3 n = normalize(normal);
	vec3 lightDirectionInverse = -normalize(lightDirection);

	float lambertian = max(dot(n, lightDirectionInverse), 0.0);

	vec3 viewDirection = vec3(0.0, 0.0, -1.0);
	vec3 reflection = reflect(lightDirectionInverse, n);
	float alpha = 10.0;
	float specular = max(pow(dot(reflection, viewDirection), alpha), 0.0);

	vec3 earthColor = texture(earthTexture, UV).rgb;
	vec3 cloudsColor = texture(cloudsTexture, UV + (time) * vec2(0.008, 0.008) ).rgb;
	vec3 finalColor = (earthColor + cloudsColor) * lightIntensity * (lambertian + specular);
	outColor = vec4(finalColor, 1.0);
}