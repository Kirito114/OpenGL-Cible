//R. RAFFIN, IUT Aix-Marseille, département Informatique, site d'Arles
//M4104c, ARCHI IN
//romain.raffin[AT]univ-amu.fr

//une classe basique pour lire les fichiers OFF

#ifndef _INC_OBJETSOFF_H
#define _INC_OBJETSOFF_H

struct indexedface { unsigned int S1, S2, S3; };

struct point3D {
	float x;
	float y;
	float z;
	float s;
	float t;
};

class Objet {

public:
	unsigned int nbsommets;
	unsigned int nbfaces;

	unsigned int id;

	struct point3D *lpoints;
	struct indexedface *lfaces;
	struct point3D min;
	struct point3D max;

	Objet();
	Objet(const Objet&);
	~Objet();
	void loadOFF(const char *nom_fichier,bool loadUV);
	void affiche();
};

#endif