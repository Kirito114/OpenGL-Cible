#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float xOffset;
uniform float yOffset;
uniform float xVibrate;
uniform float yVibrate;

void main()
{
	vec4 newPosition = vec4(position.x + xOffset + xVibrate, position.y + yOffset + yVibrate, position.z, 1.0f);
	gl_Position = projection * view * model * newPosition;
	TexCoord = vec2(texCoord.x,1.0 - texCoord.y);
}
