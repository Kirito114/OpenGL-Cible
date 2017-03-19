#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D textureCible;

void main()
{
	color = texture(textureCible,TexCoord);
	//color = vec4(TexCoord.x,TexCoord.y,0.0,1.0);
}