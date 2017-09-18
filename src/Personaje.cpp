////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Personaje.h"

Personaje::Personaje(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad) : Objeto(gestEscena, nombre, nombreMesh), _nivel(nivel), _vida(vida), _vidaMaxima(vida), _ptsAtaque(ptsAtaque), _ptsDefensa(ptsDefensa), _rangoAtaque(rangoAtaque), _velocidad(velocidad), bIdle(false), bCorriendo(false), bAtacando(false), bVivo(true), bSangrando(false), temporizadorSangre(0)
{
	/*gestorEscena = Ogre::Root::getSingleton().getSceneManager("gestorEscena");
	entidad = gestorEscena->createEntity(nombre, nombreMesh);
	entidad->setCastShadows(true);*/

	temporizadorSangre = new Ogre::Timer();

	srand(time(NULL));
}

Personaje::~Personaje(void)
{
	if(temporizadorSangre)
		delete temporizadorSangre;
}

unsigned int Personaje::contador = 0;