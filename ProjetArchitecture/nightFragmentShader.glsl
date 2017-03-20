#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D textureCible;

void main()
{
	color = texture(textureCible.x * 0.1, textureCible.y * 0.65, textureCible.z * 0.2 ,TexCoord);
}
