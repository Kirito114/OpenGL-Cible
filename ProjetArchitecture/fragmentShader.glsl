#version 330 core

in vec2 TexCoord;
in float pointTouched;
out vec4 color;

uniform sampler2D textureCible;
uniform int vision_mode;

void main()
{
	if(vision_mode == 0)
		color = texture(textureCible,TexCoord);
	else if(vision_mode == 1)
		color = texture(textureCible, TexCoord) * vec4(0.75, 0.2, 0.1, 1.0f);
	else if(vision_mode == 2)
		color = texture(textureCible, TexCoord) * vec4(0.1, 0.75, 0.2, 1.0f);
	if(pointTouched == 1)
		color *= vec4(0.7f,0.7f,0.7f,1.0f);
}
