////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef JUEGO_H_
#define JUEGO_H_

#pragma once
#include "Jugador.h"
#include "Enemigo.h"
#include "Personaje.h"
#include "FabricaEnemigos.h"
#include "Core.h"
#include <OGRE/OgrePanelOverlayElement.h>
#include <OGRE/OgreTextAreaOverlayElement.h>
#include <iostream>
#include <map>
#include <list>

/**
 * clase principal
 */
class Juego :	public Core,
				public Ogre::FrameListener,
				public Ogre::WindowEventListener,
				public OIS::KeyListener,
				public OIS::MouseListener
{
public:
	Juego(void);
	virtual ~Juego(void);

private:

    //modelos de la escena
	Ogre::SceneNode* nodoMenuPrincipal;
	Ogre::SceneNode* nodoJuego;

	Ogre::SceneNode* nodoTunel1;
	Ogre::Entity* entidadTunel1;
	Ogre::SceneNode* nodoCofre;
	Ogre::SceneNode* nodoTunel2;
	Ogre::Entity* entidadTunel2;
	Ogre::Entity* entidadCofre;

    //--------- eliminar esto -----------//
	Ogre::OverlayManager* gestorOverlay;
	Ogre::Overlay* overlay;
	Ogre::PanelOverlayElement* panel;
	Ogre::TextAreaOverlayElement* textoTitulo;
	Ogre::TextAreaOverlayElement* textoParticulas;
	Ogre::TextAreaOverlayElement* textoFps;
	//------------------------------------//

    //imagen de fondo del menu principal
	Ogre::Rectangle2D* fondo;
	Ogre::SceneNode* nodoFondo;

    //lista de enemigos
	std::list<Enemigo*> listaEnemigos;
	std::list<Enemigo*>::iterator it;

	//configuramos GUI
	MyGUI::WidgetPtr widgetMenuJuego;
	MyGUI::ButtonPtr botonMenuJuegoCreditos;
	MyGUI::ButtonPtr botonMenuJuegoInstrucciones;
	MyGUI::ButtonPtr botonMenuJuegoSalir;

	MyGUI::WidgetPtr widgetMenuPrincipal;
	MyGUI::ButtonPtr botonMenuPrincipalJugar;
	MyGUI::ButtonPtr botonMenuPrincipalCreditos;
	MyGUI::ButtonPtr botonMenuPrincipalInstrucciones;
	MyGUI::ButtonPtr botonMenuPrincipalSalir;

	MyGUI::WidgetPtr widgetMenuCreditos;
	MyGUI::ButtonPtr botonMenuCreditosVolver;

	MyGUI::WidgetPtr widgetMenuInstrucciones;
	MyGUI::ButtonPtr botonMenuInstruccionesVolver;

	MyGUI::WidgetPtr widgetPanelStats;
	MyGUI::ImageBox* imagenBarraVida;
	MyGUI::ImageBox* imagenBarraExp;
	MyGUI::TextBox* textoNivel;
	MyGUI::TextBox* textoVida;
	MyGUI::TextBox* textoExp;
	MyGUI::TextBox* textoAtaque;
	MyGUI::TextBox* textoDefensa;

    //personajes
	Jugador* jugador;
	Enemigo* enemigoCercano;
	//fabricas de enemigos
	FabricaEnemigos*  FabricaEnemigosGenerica;
	FabricaEnemigos*  FabricaNinjas;
	FabricaEnemigos*  FabricaDemonios;
	FabricaEnemigos*  FabricaGoblins;
	FabricaEnemigos*  FabricaGolems;

    //true para cerrar
	bool bSalir;
	//true si esta pausado
	bool bPausa;
	//true si hay enemigos en el sentido al que mira el jugador
	bool bHayEnemigos;
	//true si ya no estamos en el menu principal
	bool bJugando;
	//true si se presiono la tecla ESC
	bool bTeclaESC;

    //la distancia entre el enemigo mas cercano y el jugador
	float distEnemigo;
	//variable temporal de la anterior
	float tempDistEnemigo;

    //este metodo es llamado cuando se presiona una tecla
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	//este metodo es llamado cuando se suelta una tecla
	virtual bool keyReleased(const OIS::KeyEvent &arg);

    //este metodo es llamado cuando se mueve el raton
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	//este metodo es llamado cuando se presiona un boton del raton
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	//este metodo es llamado cuando se suelta un boton del raton
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    //este metodo es llamado antes de renderizar
	bool frameStarted(const Ogre::FrameEvent &evt);
	//este metodo es llamado cuando se renderizo pero no se volco a pantalla
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	//este metodo es llamado despues de renderizar
	bool frameEnded(const Ogre::FrameEvent &evt);

    //este metodo es llamado cuando se cierra la ventana
	void windowClosed(Ogre::RenderWindow* rw);

    //metodo principal que llama a los demas
	void iniciarJuego(void);
	//creamos la interfaz de usuario
	void crearGUI(void);
	//creamos la escena
	void crearEscena(void);
	//creamos el jugador y las fabricas de enemigos
	void prepararPersonajes(void);
	//actualizamos el estado de los enemigos
	void actualizarEnemigos(Ogre::Real tiempoUltimoFrame);
	//actualizamos el entorno y los enemigos
	void actualizarEscenario(void);
	//refrescamos la interfaz
	void actualizarGUI();

    //cambia el valor de bSalir a false
	void salir(MyGUI::WidgetPtr _sender);
	//cambia el valor de bJugando a true
	void jugar(MyGUI::WidgetPtr _sender);
	//muestra los creditos
	void mostrarCreditos(MyGUI::WidgetPtr _sender);
	//muestra las instrucciones
	void mostrarInstrucciones(MyGUI::WidgetPtr _sender);
	//regresa al menu anterior
	void volverAlMenu(MyGUI::WidgetPtr _sender);
};

#endif /* JUEGO_H_ */