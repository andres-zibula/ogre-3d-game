////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef ENEMIGO_H_
#define ENEMIGO_H_

#pragma once
class Jugador;
#include "Personaje.h"
#include "MovableText.h"
#include "Jugador.h"

/**
 * clase encargada de gestionar a los enemigos
 */
class Enemigo :
	public Personaje
{
public:
	Enemigo(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad, const Ogre::Vector3& escala);
	virtual ~Enemigo(void);

	void setDireccion(direccion dir);
	void setCorriendo(bool correr);
	void setIdle(bool idle);

	void actualizarBarraVida();
	void actualizarTexto();
	void actualizar(Jugador* jugador, Ogre::Real &tiempoUltimoFrame);
	void atacar(Jugador* jugador, Ogre::Real &tiempoUltimoFrame);
	void sangrar();

private:
	float distJugador;

	Ogre::String tipoEnemigo;

	Ogre::AnimationState* animCorrer;
	Ogre::AnimationState* animIdle;
	Ogre::AnimationState* animAtacar;
	Ogre::AnimationState* animMorir;

	Ogre::SceneNode* nodoTexto;
	Ogre::MovableText* barraTexto;

	Ogre::BillboardSet* bbsBarraVida;
	Ogre::Billboard* bbBarraVida;
	Ogre::SceneNode* nodoBarraVida;
};

#endif /* ENEMIGO_H_ */
