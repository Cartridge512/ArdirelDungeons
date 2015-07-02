#version 420

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 vertColor;

out vec4 varyingColor;

uniform mat4 worldMatrix;

void main()
{
	gl_Position = worldMatrix * vec4(position, 0, 1);

	varyingColor = vec4(vertColor, 1);
}