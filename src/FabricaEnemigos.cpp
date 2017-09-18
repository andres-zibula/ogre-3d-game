////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "FabricaEnemigos.h"

FabricaEnemigos::FabricaEnemigos()
{
}

FabricaEnemigos::FabricaEnemigos(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad, const Ogre::Vector3& escala) : gestorEscena(gestEscena), nombreEnem(nombre), vidaEnem(vida), ptsAtaqueEnem(ptsAtaque), ptsDefensaEnem(ptsDefensa), rangoAtaqueEnem(rangoAtaque), velocidadEnem(velocidad), _escala(escala)
{
}

FabricaEnemigos::~FabricaEnemigos(void)
{
}

Enemigo* FabricaEnemigos::crear(int nivel)
{
	return new Enemigo(gestorEscena, nombreEnem, nombreEnem + Ogre::String(".mesh"), nivel, vidaEnem, ptsAtaqueEnem, ptsDefensaEnem, rangoAtaqueEnem, velocidadEnem, _escala);
}

Enemigo* FabricaEnemigos::crear(int nivel, float x, float y, float z)
{
	Enemigo* nuevoEnemigo = new Enemigo(gestorEscena, nombreEnem, nombreEnem + Ogre::String(".mesh"), nivel, vidaEnem, ptsAtaqueEnem, ptsDefensaEnem, rangoAtaqueEnem, velocidadEnem, _escala);
	nuevoEnemigo->setPosicion(x, y, z);
	return nuevoEnemigo;
}

void FabricaEnemigos::destruir(Enemigo* enemigo)
{
	delete enemigo;
}
