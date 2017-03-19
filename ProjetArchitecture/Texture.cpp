#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"

Texture::Texture() : id(0), img(nullptr), width(0), height(0)
{}

bool Texture::load(const char * filename)
{
	glGenTextures(1, &id);
	img = stbi_load(filename, &width, &height, &opp, 0);
	if (img != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		if (opp == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

		delete[] img;
		return true;
	}
	else
		return false;
}

void Texture::use()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unuse()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::define_filtering(GLint mode_min, GLint mode_mag)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode_mag);
}

void Texture::define_looping(GLint mode_axe_s, GLint mode_axe_t)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_axe_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_axe_t);
}

void Texture::define_mixing(GLint mode)
{
	if (mode == GL_DECAL || mode == GL_MODULATE)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
}
