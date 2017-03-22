#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;
out float pointTouched;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float xOffset;
uniform float yOffset;
uniform float xVibrate;
uniform float yVibrate;
uniform int nbDeformations;
uniform float[50] xDeformations;
uniform float[50] yDeformations;
uniform bool wasTouched;

void main()
{
	vec4 newPosition;
	bool nearHole = false;
	for (int i = 0; i < nbDeformations && !nearHole; i++)
	{
		float distance = (pow((position.x - xDeformations[i]), 2) + pow((position.y - yDeformations[i]), 2));
		if ((distance < 50) && wasTouched)
		{
			newPosition = vec4(position.x + xOffset + xVibrate, position.y + yOffset + yVibrate, - 50 + distance, 1.0f);
			nearHole = true;
			pointTouched = 1;
		}
	}
	if (!nearHole)
	{
		newPosition = vec4(position.x + xOffset + xVibrate, position.y + yOffset + yVibrate, position.z, 1.0f);
		pointTouched = 0;
	}
	gl_Position = projection * view * model * newPosition;
	TexCoord = vec2(texCoord.x,1.0 - texCoord.y);
}
