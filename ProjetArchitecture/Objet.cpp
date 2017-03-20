//#include <errno.h>
//#include <cstring>
//#include <unistd.h>
//#include <cstdlib>
//#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "Objet.h"

Objet::Objet() : nbsommets(0),nbfaces(0),id(0)
{}

Objet::~Objet() {
	delete[] lpoints;
	delete[] lfaces;
}

void Objet::loadOFF(const char * nom_fichier,bool loadUV)
{
	unsigned int i;
	std::ifstream fichier(nom_fichier, std::ios::in);
	unsigned int n;

	std::string magic;

	if (fichier)
	{
		fichier >> magic;

		if (magic != "OFF") {
			std::cerr << "Erreur dans l'en tête du fichier OFF" << std::endl;
			exit(1);
		}

		fichier >> nbsommets >> nbfaces >> i;

		//Allocation des listes de données
		lfaces = new struct indexedface[nbfaces];
		lpoints = new struct point3D[nbsommets];

		//Remplissage de la liste de points
		if (loadUV)
		{
			for (i = 0; i<nbsommets; i++)
			{
				fichier >> lpoints[i].x >> lpoints[i].y >> lpoints[i].z >> lpoints[i].s >> lpoints[i].t;
				if (min.x>lpoints[i].x)
					min.x = lpoints[i].x;
				if (min.y>lpoints[i].y)
					min.y = lpoints[i].y;
				if (min.z>lpoints[i].z)
					min.z = lpoints[i].z;

				if (max.x<lpoints[i].x)
					max.x = lpoints[i].x;
				if (max.y<lpoints[i].y)
					max.y = lpoints[i].y;
				if (max.z<lpoints[i].z)
					max.z = lpoints[i].z;
			}
		}
		else
		{
			for (i = 0; i<nbsommets; i++)
			{
				fichier >> lpoints[i].x >> lpoints[i].y >> lpoints[i].z;
				lpoints[i].s = 0;
				lpoints[i].t = 0;
				if (min.x>lpoints[i].x)
					min.x = lpoints[i].x;
				if (min.y>lpoints[i].y)
					min.y = lpoints[i].y;
				if (min.z>lpoints[i].z)
					min.z = lpoints[i].z;

				if (max.x<lpoints[i].x)
					max.x = lpoints[i].x;
				if (max.y<lpoints[i].y)
					max.y = lpoints[i].y;
				if (max.z<lpoints[i].z)
					max.z = lpoints[i].z;
			}
		}
		

		//Remplissage de la liste de faces
		for (i = 0; i<nbfaces; i++)
		{
			fichier >> n >> lfaces[i].S1 >> lfaces[i].S2 >> lfaces[i].S3;
			if (n != 3) {
				std::cerr << "Erreur au chargement des faces, non triangulaires" << std::endl;
				exit(1);
			}
		}
	}
	else
	{
		std::cerr << "Impossible d'ouvrir le fichier" << std::endl;
	}

	
}

void Objet::affiche()
{
	unsigned int i;
	std::cerr << "Objet : " << nbsommets << " sommets " << nbfaces << " faces" << std::endl;

	std::cerr << "SOMMETS" << std::endl;
	for (i = 0; i<20; i++)
		std::cerr << "Sommets " << i << "[" << std::setprecision(5) << lpoints[i].x << " " << lpoints[i].y << " " << lpoints[i].z << "] " << lpoints[i].s << " " << lpoints[i].t << std::endl;

	std::cerr << "FACES" << std::endl;
	for (i = 0; i<nbfaces; i++)
		std::cerr << "Face " << i << " " << lfaces[i].S1 << " " << lfaces[i].S2 << " " << lfaces[i].S3 << std::endl;
}