////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef FABRICAENEMIGOS_H_
#define FABRICAENEMIGOS_H_

#pragma once
#include "Enemigo.h"

class FabricaEnemigos
{
public:
	FabricaEnemigos();
	FabricaEnemigos(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad, const Ogre::Vector3& escala = Ogre::Vector3::UNIT_SCALE);
	virtual ~FabricaEnemigos(void);

	Enemigo* crear(int nivel);
	Enemigo* crear(int nivel, float x, float y, float z);
	void destruir(Enemigo* enemigo);

private:
	unsigned int contador;

	Ogre::SceneManager* gestorEscena;

	Ogre::String nombreEnem;
	int vidaEnem;
	int ptsAtaqueEnem;
	int ptsDefensaEnem;
	int rangoAtaqueEnem;
	float velocidadEnem;
	Ogre::Vector3 _escala;
};

#endif /* FABRICAENEMIGOS_H_ */
