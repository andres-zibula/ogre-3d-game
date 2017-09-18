////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef CORE_H_
#define CORE_H_

#pragma once
#include <OGRE/Ogre.h>
#include <OIS.h>
#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>
#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_mixer.h>
#elif __linux__
	#include <SDL/SDL.h>
	#include <SDL/SDL_mixer.h>
#endif

/**
 * Core se encarga de inicializar los diferentes sistemas
 */
class Core
{
public:
	Core(void);
	~Core(void);

protected:
	//configuramos Ogre
	Ogre::ConfigFile archivoRecursosConfig;
	Ogre::LogManager* logManager;

	Ogre::Log* logSistema;
	Ogre::Log* logJuego;

	Ogre::Root* raiz;

	Ogre::RenderWindow* ventana;

	Ogre::SceneManager* gestorEscena;
	Ogre::Camera* camara;
	Ogre::Viewport* viewport;

	//configuramos OIS
	OIS::ParamList listaParamOIS;
	OIS::InputManager* gestorEntrada;
	OIS::Keyboard* teclado;
	OIS::Mouse* mouse;

	size_t hndVentana;
	std::ostringstream strHndVentana;

	//configuramos MyGUI
	MyGUI::OgrePlatform* myGUIPlatform;
	MyGUI::Gui* myGUI;

private:
	//inicializamos Ogre
	bool configurarOgre(void);
	//Inicializamos el sistema de entrada
	void configurarOIS(void);
	//configuramos la camara y el viewport
	void configurarCamara(void);
	//detectamos la localizacion de los recursos
	void cargarRecursos(void);
	//inicializamos MyGUI
	void configurarGUI(void);
	//inicializamos el sistema de sonido
	void configurarSonido(void);
};

#endif /* CORE_H_ */
