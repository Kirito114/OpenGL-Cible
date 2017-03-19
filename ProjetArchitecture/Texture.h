#ifndef DEF_TEXTURE
#define DEF_TEXTURE

#ifndef BYTE
typedef unsigned char byte;
#endif

#include <GL/glew.h>
#include <iostream>


class Texture
{
private:
	GLuint id;
	byte * img;
	int width, height, opp;
public:
	Texture();
	bool load(const char * nom_fichier);
	void use();
	void unuse();
	void define_filtering(GLint mode_min, GLint mode_mag);
	void define_looping(GLint mode_axe_s, GLint mode_axe_t);
	void define_mixing(GLint mode);
};

#endif

