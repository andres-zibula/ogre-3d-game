////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef JUGADOR_H_
#define JUGADOR_H_

#pragma once
class Enemigo;
#include "Personaje.h"
#include "Enemigo.h"
#include <map>

class Jugador : public Personaje
{
public:
	Jugador(Ogre::SceneManager* gestEscena, Ogre::Camera* camera, OIS::Keyboard* oisTeclado);
	~Jugador();

	const Ogre::SceneNode* getNodoJugador() const {return nodoJugador;}
	const int& getExp() const {return _exp;}
	const int& getExpMaxima() const {return _expMaxima;}

	void aumentarExp(int exp) {_exp += exp;}
	void setDireccion(direccion dir);
	void setCorriendo(bool correr);
	void setIdle(bool idle);
	void necesitaActualizarGUI(bool bBool) { bNecesitaActualizarGUI = bBool;}
	void actualizar(Enemigo* enemigo, float distEnemigo, Ogre::Real tiempoDesdeUltimoFrame);
	void atacar(Enemigo* enemigo, float distEnemigo, Ogre::Real tiempoUltimoFrame);
	void sangrar();
	void saltar(Ogre::Real tiempoUltimoFrame);
	void subirNivel();

	bool estaSubiendoNivel() {return bSubiendoNivel;}
	bool estaSaltando() {return bSaltando;}
	bool necesitaActualizarGUI() {return bNecesitaActualizarGUI;}

private:
	Ogre::Camera* camara;
	Ogre::SceneNode* nodoCamara;
	Ogre::SceneNode* nodoJugador;
	Ogre::SceneNode* nodoParticulaSubirNivel;

	Ogre::Entity* espada1;
	Ogre::Entity* espada2;

	Ogre::AnimationState* animCorrerInf;
	Ogre::AnimationState* animCorrerSup;
	Ogre::AnimationState* animIdleInf;
	Ogre::AnimationState* animIdleSup;
	Ogre::AnimationState* animAtacar;
	Ogre::AnimationState* animSaltarPrincipio;
	Ogre::AnimationState* animSaltarFinal;
	Ogre::AnimationState* animSaltarBucle;

	Ogre::Timer* temporizadorSubirNivel;
	Ogre::ParticleSystem* particulaSubirNivel;

	OIS::Keyboard* teclado;

	bool bTeclaD;
	bool bTeclaA;
	bool bTeclaW;
	bool bTeclaS;
	bool bTeclaEspacio;
	bool bInterpolando;

	bool bSaltando;
	bool bBajando;
	bool bNecesitaActualizarGUI;
	bool bSubiendoNivel;

	Ogre::Vector3 camPosInicial;
	Ogre::Vector3 camPosFinal;
	float camTiempoActual;
	float camTiempoFinal;
	float camTiempoInterp;

	float altura;

	int _exp;
	int _expMaxima;
};


#endif /* JUGADOR_H_ */
