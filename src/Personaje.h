////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef PERSONAJE_H_
#define PERSONAJE_H_

#pragma once
#include "Objeto.h"
#include <OGRE/Ogre.h>
#include <OIS.h>
#include <ctime>
#include <cstdlib>

/**
 * esta clase se encarga de proveer los atributos y funciones mas basicos que necesita el jugador y los enemigos
 */
class Personaje :
	public Objeto
{
public:
	Personaje(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad);
	virtual ~Personaje(void);

	enum direccion {ADELANTE, ATRAS};

	const direccion& getDireccion() const {return _direccion;}
	const int& getNivel() const {return _nivel;}
	const int& getVida() const {return _vida;}
	const int& getVidaMaxima() const {return _vidaMaxima;}
	const int& getPtsAtaque() const {return _ptsAtaque;}
	const int& getPtsDefensa() const {return _ptsDefensa;}
	const int& getRangoAtaque() const {return _rangoAtaque;}
	const float& getVelocidad() const {return _velocidad;}

	const bool& estaCorriendo() const {return bCorriendo;}
	const bool& estaIdle() const {return bIdle;}
	const bool& estaAtacando() const {return bAtacando;}
	const bool& estaVivo() const {return bVivo;}
	const bool& estaSangrando() const {return bSangrando;}
	const bool tieneVida() const {return static_cast<bool>(_vida);}

	void setVida(int vida) {_vida = vida;}
	void setVidaMaxima(int vidaMaxima) {_vidaMaxima = vidaMaxima;}
	void setPtsAtaque(int ptsAtaque) {_ptsAtaque = ptsAtaque;}
	void setPtsDefensa(int ptsDefensa)  {_ptsDefensa = ptsDefensa;}
	void decrementarVida(int vida) {_vida = vida < 0 ? _vida : (_vida - vida < 0 ? 0 : _vida - vida);}
	virtual void setDireccion(direccion dir) = 0;
	virtual void setCorriendo(bool correr) = 0;
	virtual void setIdle(bool idle) = 0;
	virtual void sangrar() = 0;

protected:
	static unsigned int contador;
	int numeroRand;

	Ogre::SceneNode* nodoParticulaSangre;

	std::map<unsigned short int, Ogre::AnimationState*> mapAnimAtacar;

	direccion _direccion;
	Ogre::Vector3 vecDir;

	int _nivel;
	int _vida;
	int _vidaMaxima;
	int _ptsAtaque;
	int _ptsDefensa;
	int _rangoAtaque;
	float _velocidad;

	bool bIdle;
	bool bCorriendo;
	bool bAtacando;
	bool bVivo;
	bool bSangrando;

	Ogre::Timer* temporizadorSangre;
	Ogre::ParticleSystem* particulaSangre;
};

#endif /* PERSONAJE_H_ */
