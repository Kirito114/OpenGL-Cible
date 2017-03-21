#version 330 core

uniform vec3 color;
out vec4 fragColor;

uniform int vision_mode;

void main()
{
	if(vision_mode == 0)
		fragColor = vec4(color, 1.0f);
	else if(vision_mode == 1)
		fragColor =  vec4(color,1.0f) * vec4(0.75f, 0.2f, 0.1f, 1.0f);
	else if(vision_mode == 2)
		fragColor =  vec4(color,1.0f) * vec4(0.1f, 0.75f, 0.2f, 1.0f);
}