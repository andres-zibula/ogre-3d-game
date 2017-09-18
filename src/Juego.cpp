////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Juego.h"

Juego::Juego(void) : fondo(0), jugador(0), enemigoCercano(0), FabricaEnemigosGenerica(0), FabricaNinjas(0), FabricaDemonios(0), FabricaGoblins(0), FabricaGolems(0),  bSalir(false), bPausa(true), bJugando(false), bTeclaESC(false)
{
    //si los subsistemas se iniciaron correctamente procedemos a iniciar el juego
	if(raiz)
	{
		iniciarJuego();
	}
}

Juego::~Juego(void)
{
	if(fondo)
		delete fondo;

	if(jugador)
		delete jugador;

	if(FabricaEnemigosGenerica)
	{
		for(it = listaEnemigos.begin(); it != listaEnemigos.end();)
		{
			FabricaEnemigosGenerica->destruir(*it);
			it = listaEnemigos.erase(it);
		}
		delete FabricaEnemigosGenerica;
	}

	if(FabricaNinjas)
		delete FabricaNinjas;

	if(FabricaDemonios)
		delete FabricaDemonios;

    if(FabricaGoblins)
		delete FabricaGoblins;

    if(FabricaGolems)
		delete FabricaGolems;
}

void Juego::iniciarJuego()
{
    //randomizamos la semilla
	srand(time(NULL));

	crearEscena();

	crearGUI();

	raiz->addFrameListener(this);
	Ogre::WindowEventUtilities::addWindowEventListener(ventana, this);
	teclado->setEventCallback(this);
	mouse->setEventCallback(this);

	raiz->startRendering();
}

void Juego::crearGUI(void)
{
    //cargamos el archivo de configuracion de interfaz
	MyGUI::LayoutManager::getInstance().loadLayout("GUI.layout");
	MyGUI::LayerManager::getInstancePtr()->resizeView(MyGUI::RenderManager::getInstancePtr()->getViewSize());

    //cargamos los widgets
	widgetMenuJuego = myGUI->findWidget<MyGUI::Widget>("MenuJuego");
	botonMenuJuegoCreditos = myGUI->findWidget<MyGUI::Button>("menuJuegoCreditos");
	botonMenuJuegoInstrucciones = myGUI->findWidget<MyGUI::Button>("menuJuegoInstrucciones");
	botonMenuJuegoSalir = myGUI->findWidget<MyGUI::Button>("menuJuegoSalir");
	botonMenuJuegoCreditos->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::mostrarCreditos);
	botonMenuJuegoInstrucciones->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::mostrarInstrucciones);
	botonMenuJuegoSalir->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::salir);

	widgetMenuPrincipal = myGUI->findWidget<MyGUI::Widget>("MenuPrincipal");
	botonMenuPrincipalJugar = myGUI->findWidget<MyGUI::Button>("menuPrincipalJugar");
	botonMenuPrincipalCreditos = myGUI->findWidget<MyGUI::Button>("menuPrincipalCreditos");
	botonMenuPrincipalInstrucciones = myGUI->findWidget<MyGUI::Button>("menuPrincipalInstrucciones");
	botonMenuPrincipalSalir = myGUI->findWidget<MyGUI::Button>("menuPrincipalSalir");
	botonMenuPrincipalJugar->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::jugar);
	botonMenuPrincipalCreditos->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::mostrarCreditos);
	botonMenuPrincipalInstrucciones->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::mostrarInstrucciones);
	botonMenuPrincipalSalir->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::salir);

	widgetMenuCreditos = myGUI->findWidget<MyGUI::Widget>("MenuCreditos");
	botonMenuCreditosVolver = myGUI->findWidget<MyGUI::Button>("menuCreditosVolver");
	botonMenuCreditosVolver->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::volverAlMenu);

	widgetMenuInstrucciones = myGUI->findWidget<MyGUI::Widget>("MenuInstrucciones");
	botonMenuInstruccionesVolver = myGUI->findWidget<MyGUI::Button>("menuInstruccionesVolver");
	botonMenuInstruccionesVolver->eventMouseButtonClick += MyGUI::newDelegate(this, &Juego::volverAlMenu);

	widgetPanelStats = myGUI->findWidget<MyGUI::Widget>("panelStats");
	imagenBarraVida = myGUI->findWidget<MyGUI::ImageBox>("barraVida");
	imagenBarraVida->setImageCoord(MyGUI::IntCoord(0, 0, 50, 30));
	imagenBarraExp = myGUI->findWidget<MyGUI::ImageBox>("barraExp");
	imagenBarraExp->setImageCoord(MyGUI::IntCoord(0, 0, 50, 30));
	textoNivel = myGUI->findWidget<MyGUI::TextBox>("textoNivel");
	textoVida = myGUI->findWidget<MyGUI::TextBox>("textoVida");
	textoExp = myGUI->findWidget<MyGUI::TextBox>("textoExp");
	textoAtaque = myGUI->findWidget<MyGUI::TextBox>("textoAtaque");
	textoDefensa = myGUI->findWidget<MyGUI::TextBox>("textoDefensa");


	widgetMenuJuego->setVisible(false);
	widgetMenuPrincipal->setVisible(true);
	widgetMenuCreditos->setVisible(false);
	widgetMenuInstrucciones->setVisible(false);
	widgetPanelStats->setVisible(false);

	//overlays
	gestorOverlay = Ogre::OverlayManager::getSingletonPtr();
	overlay = gestorOverlay->getByName("OverlayInfo");

	panel = static_cast<Ogre::PanelOverlayElement*>(gestorOverlay->getOverlayElement("panelInfo"));
	textoTitulo = static_cast<Ogre::TextAreaOverlayElement*>(gestorOverlay->getOverlayElement("tituloInfo"));
	textoParticulas = static_cast<Ogre::TextAreaOverlayElement*>(gestorOverlay->getOverlayElement("particulasInfo"));
	textoFps = static_cast<Ogre::TextAreaOverlayElement*>(gestorOverlay->getOverlayElement("fpsInfo"));

	textoTitulo->setCaption(Ogre::String("Informacion"));
	overlay->show();
}

void Juego::actualizarGUI()
{
	Ogre::Real ratio = (Ogre::Real) jugador->getVida() / (Ogre::Real) jugador->getVidaMaxima();

	if (ratio < 0.0f)
		ratio = 0.0f;

    //actualizamos la barra de vida
	imagenBarraVida->setImageCoord(MyGUI::IntCoord((100 - (ratio*100)) / 2, 0, 50 + ((100 - (ratio*100)) / 2), 30));

	ratio = (Ogre::Real) jugador->getExp() / (Ogre::Real) jugador->getExpMaxima();

	if (ratio < 0.0f)
		ratio = 0.0f;

    //actualizamos la barra de experiencia
	imagenBarraExp->setImageCoord(MyGUI::IntCoord((100 - (ratio*100)) / 2, 0, 50 + ((100 - (ratio*100)) / 2), 30));

    //actualizamos los textos
	textoNivel->setCaption("Nivel: " + Ogre::StringConverter::toString(jugador->getNivel()));
	textoVida->setCaption("Vida: " + Ogre::StringConverter::toString(jugador->getVida()) + "/" +  Ogre::StringConverter::toString(jugador->getVidaMaxima()));
	textoExp->setCaption("Exp: " + Ogre::StringConverter::toString(jugador->getExp()) + "/" +  Ogre::StringConverter::toString(jugador->getExpMaxima()));
	textoAtaque->setCaption("Ataque: " + Ogre::StringConverter::toString(jugador->getPtsAtaque()));
	textoDefensa->setCaption("Defensa: " + Ogre::StringConverter::toString(jugador->getPtsDefensa()));

    //ya actualizamos la GUI
	jugador->necesitaActualizarGUI(false);
}

void Juego::salir(MyGUI::WidgetPtr _sender)
{
	bSalir = true;
}

void Juego::jugar(MyGUI::WidgetPtr _sender)
{
    //ocultamos el menu principal y mostramos el escenario

	bJugando = true;
	bPausa = false;
	nodoMenuPrincipal->setVisible(false);
	widgetMenuPrincipal->setVisible(false);
	widgetPanelStats->setVisible(true);
	nodoJuego->setVisible(true);
}

void Juego::mostrarCreditos(MyGUI::WidgetPtr _sender)
{
	if(widgetMenuPrincipal->getVisible())
		widgetMenuPrincipal->setVisible(false);
	if(widgetMenuJuego->getVisible())
		widgetMenuJuego->setVisible(false);

	widgetMenuCreditos->setVisible(true);
}

void Juego::mostrarInstrucciones(MyGUI::WidgetPtr _sender)
{
	if(widgetMenuPrincipal->getVisible())
		widgetMenuPrincipal->setVisible(false);
	if(widgetMenuJuego->getVisible())
		widgetMenuJuego->setVisible(false);

	widgetMenuInstrucciones->setVisible(true);
}

void Juego::volverAlMenu(MyGUI::WidgetPtr _sender)
{
	if(bJugando)
		widgetMenuJuego->setVisible(true);
	else
		widgetMenuPrincipal->setVisible(true);

	if(widgetMenuCreditos->getVisible())
		widgetMenuCreditos->setVisible(false);
	if(widgetMenuInstrucciones->getVisible())
		widgetMenuInstrucciones->setVisible(false);
}

void Juego::crearEscena(void)
{
	gestorEscena->setShadowUseInfiniteFarPlane(false);
	//camara->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);

	nodoMenuPrincipal = gestorEscena->getRootSceneNode()->createChildSceneNode("menuPrincipal");
	nodoJuego = gestorEscena->getRootSceneNode()->createChildSceneNode("juego");
	gestorEscena->setAmbientLight(Ogre::ColourValue(0.37f, 0.37f, 0.37f, 1.0f));

	//creamos el tunel
	nodoTunel1 = nodoJuego->createChildSceneNode();
	entidadTunel1 = gestorEscena->createEntity("cueva1", "Cube.001.mesh");
	entidadTunel1->setCastShadows(false);
	nodoTunel1->attachObject(entidadTunel1);
	//nodoTunel1->showBoundingBox(true);
	nodoTunel1->yaw(Ogre::Radian(Ogre::Degree(-90)));

	//creamos el cofre
	/*nodoCofre = nodoTunel1->createChildSceneNode("nodoCofre");
	entidadCofre = gestorEscena->createEntity("cofre", "stone_coffin.mesh");
	nodoCofre->attachObject(entidadCofre);
	nodoCofre->scale(Ogre::Vector3(0.05f));*/
	//nodoCofre->setPosition(/*nodoCueva1->convertLocalToWorldPosition(nodoCueva1->getPosition()) + */ Ogre::Vector3(20.0f, 0.0f, 4.0f));


	nodoTunel2 = nodoJuego->createChildSceneNode();
	entidadTunel2 = gestorEscena->createEntity("cueva2", "Cube.001.mesh");
	entidadTunel2->setCastShadows(false);
	nodoTunel2->attachObject(entidadTunel2);
	//nodoTunel2->showBoundingBox(true);
	nodoTunel2->yaw(Ogre::Radian(Ogre::Degree(-90)));
	nodoTunel2->setPosition(-130, 0, 0);

	//creamos las luces
	Ogre::Light* luzPunto1 = gestorEscena->createLight("luzPunto1");
	luzPunto1->setType(Ogre::Light::LT_POINT);
	luzPunto1->setDiffuseColour(1.0, 0.5, 0.0);
	luzPunto1->setSpecularColour(1.0, 1.0, 0.0);
	luzPunto1->setAttenuation(160.0f, 1.0f, 0.027f, 0.0028f);
	Ogre::SceneNode* nodoLuzPunto1 = nodoTunel1->createChildSceneNode();
	nodoLuzPunto1->attachObject(luzPunto1);
	luzPunto1->setPosition(0, 0, 0);
	Ogre::ParticleSystem* particulas1 = gestorEscena->createParticleSystem("fuego1", "fuego");
	Ogre::SceneNode* nodoParticulaFuego1 = nodoLuzPunto1->createChildSceneNode();
	nodoParticulaFuego1->attachObject(particulas1);
	nodoLuzPunto1->setPosition(-30, 10, 0); //como se roto 90 grados el tunel

	Ogre::Light* luzPunto2 = gestorEscena->createLight("luzPunto2");
	luzPunto2->setType(Ogre::Light::LT_POINT);
	luzPunto2->setDiffuseColour(1.0, 0.5, 0.0);
	luzPunto2->setSpecularColour(1.0, 1.0, 0.0);
	luzPunto2->setAttenuation(160.0f, 1.0f, 0.027f, 0.0028f);
	Ogre::SceneNode* nodoLuzPunto2 = nodoTunel2->createChildSceneNode();
	nodoLuzPunto2->attachObject(luzPunto2);
	luzPunto2->setPosition(0, 0, 0);
	Ogre::ParticleSystem* particulas2 = gestorEscena->createParticleSystem("fuego2", "fuego");
	Ogre::SceneNode* nodoParticulaFuego2 = nodoLuzPunto2->createChildSceneNode();
	nodoParticulaFuego2->attachObject(particulas2);
	nodoLuzPunto2->setPosition(-30, 10, 0); //como se roto 90 grados el tunel

	//creamos el fondo del menu principal
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("Fondo", "General");
	material->getTechnique(0)->getPass(0)->createTextureUnitState("Foothills-of-Ariloa.jpg");
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setScrollAnimation(-0.04f, 0.01f);

	fondo = new Ogre::Rectangle2D(true);
	fondo->setCorners(-1.0, 1.0, 1.0, -1.0);
	fondo->setMaterial("Fondo");

	fondo->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	fondo->setBoundingBox(aabInf);

	nodoFondo = nodoMenuPrincipal->createChildSceneNode("FondoMenu");
	nodoFondo->attachObject(fondo);

	//creamos los personajes
	prepararPersonajes();

	nodoJuego->setVisible(false);
}

void Juego::prepararPersonajes(void)
{
	jugador = new Jugador(gestorEscena, camara, teclado);
	//jugador->setPosicion(0, 0, 0);
	gestorEscena->getSceneNode("juego")->createChildSceneNode("enemigos");
	FabricaEnemigosGenerica = new FabricaEnemigos();
	FabricaNinjas = new FabricaEnemigos(gestorEscena, Ogre::String("ninja"), 1, 40, 13, 7, 4, 7.5f);
	FabricaDemonios = new FabricaEnemigos(gestorEscena, Ogre::String("demonio"), 1, 40, 19, 9, 6, 7.5f, Ogre::Vector3(6));
	FabricaGoblins = new FabricaEnemigos(gestorEscena, Ogre::String("goblin"), 1, 40, 19, 9, 6, 7.5f, Ogre::Vector3(4));
	FabricaGolems = new FabricaEnemigos(gestorEscena, Ogre::String("golem"), 1, 60, 19, 9, 9, 7.5f, Ogre::Vector3(5));
}

void Juego::actualizarEnemigos(Ogre::Real tiempoUltimoFrame)
{
	enemigoCercano = NULL;
	for(it = listaEnemigos.begin(); it != listaEnemigos.end(); ++it)
	{
		//si el enemigo tiene vida
		if((*it)->tieneVida())
		{
			distEnemigo = (*it)->getNodo()->getPosition().squaredDistance(jugador->getNodoJugador()->getPosition());
			enemigoCercano = (*it);
			break;
		}
	}

	for(it = listaEnemigos.begin(); it != listaEnemigos.end();)
	{
	    //actualizamos el enemigo
		(*it)->actualizar(jugador, tiempoUltimoFrame);

        //si tiene vida y si el jugador no tiene el mismo sentido que el enemigo
		if((*it)->tieneVida() && (*it)->getDireccion() != jugador->getDireccion())
		{
			tempDistEnemigo = (*it)->getNodo()->getPosition().squaredDistance(jugador->getNodoJugador()->getPosition());
			if(distEnemigo > tempDistEnemigo)
			{
				distEnemigo = tempDistEnemigo;
				enemigoCercano = *it;
			}
		}

		if(!(*it)->estaVivo())
		{
		    //liberamos la memoria del objeto
			FabricaEnemigosGenerica->destruir(*it);
			//borramos el objeto de la lista
			it = listaEnemigos.erase(it);
		}
		else
			it++;
	}
}

void Juego::actualizarEscenario(void)
{
    //reseteamos la variable bHayEnemigos y posEnemigo
	bHayEnemigos = false;
	Ogre::Real posEnemigo = 0.0f;
	Ogre::Real posJugador = jugador->getNodoJugador()->getPosition().x;

    //necesitamos saber si hay MAS enemigos delante del jugador (en x >  0)
	if(posJugador >= 0)
	{
		if(!listaEnemigos.empty())
		{
			for(it = listaEnemigos.begin(); it != listaEnemigos.end() && !bHayEnemigos; ++it)
			{
				posEnemigo = (*it)->getPosicion().x;
				if(posEnemigo > 0 && posJugador < posEnemigo)
				{
					bHayEnemigos = true;
				}
			}
		}

        //si no hay, creamos uno aleatorio, cada 100 unidades de distancia aumenta en un nivel de dificultad
		if(!bHayEnemigos)
		{
			int numeroRand = rand() % 8;
			if(numeroRand == 0)
				listaEnemigos.push_back(FabricaGolems->crear((posJugador / 100) < 1 ? 1 : posJugador / 100,posJugador+45, 0, 0));
			else if(numeroRand <= 2)
				listaEnemigos.push_back(FabricaGoblins->crear((posJugador / 100) < 1 ? 1 : posJugador / 100, posJugador+45, 0, 0));
			else if(numeroRand <= 4)
				listaEnemigos.push_back(FabricaDemonios->crear((posJugador / 100) < 1 ? 1 : posJugador / 100, posJugador+45, 0, 0));
			else
				listaEnemigos.push_back(FabricaNinjas->crear((posJugador / 100) < 1 ? 1 :posJugador / 100, posJugador+45, 0, 0));

			//std::cout << "enemigo creado" << std::endl;
		}

        //si la posicion del jugador es mayor que el tunel1 en la mitad
		if(posJugador > nodoTunel1->getPosition().x+65)
		{
			nodoTunel2->setPosition(nodoTunel1->getPosition());
			nodoTunel1->setPosition(nodoTunel2->getPosition()+Ogre::Vector3(130, 0, 0));
		}
		//de lo contrario, si la posicion del jugador es menor que la del tunel2 a la mitad
		else if(posJugador < nodoTunel2->getPosition().x+65)
		{
			nodoTunel1->setPosition(nodoTunel2->getPosition());
			nodoTunel2->setPosition(nodoTunel1->getPosition()+Ogre::Vector3(-130, 0, 0));
		}
	}
	//necesitamos saber si hay MAS enemigos delante del jugador (en x <=  0)
	else
	{
		if(!listaEnemigos.empty())
		{
			for(it = listaEnemigos.begin(); it != listaEnemigos.end() && !bHayEnemigos; ++it)
			{
				posEnemigo = (*it)->getPosicion().x;
				if(posEnemigo < 0 && posJugador > posEnemigo)
				{
					bHayEnemigos = true;
				}
			}
		}

        //si no hay, creamos uno aleatorio, cada 100 unidades de distancia aumenta en un nivel de dificultad
		if(!bHayEnemigos)
		{
			int numeroRand = rand() % 8;
			if(numeroRand == 0)
				listaEnemigos.push_back(FabricaGolems->crear((posJugador / -100) < 1 ? 1 : posJugador / -100, posJugador-45, 0, 0));
			else if(numeroRand <= 2)
				listaEnemigos.push_back(FabricaGoblins->crear((posJugador / -100) < 1 ? 1 : posJugador / -100, posJugador-45, 0, 0));
			else if(numeroRand <= 4)
				listaEnemigos.push_back(FabricaDemonios->crear((posJugador / -100) < 1 ? 1 : posJugador / -100, posJugador-45, 0, 0));
			else
				listaEnemigos.push_back(FabricaNinjas->crear((posJugador / -100) < 1 ? 1 : posJugador / -100, posJugador-45, 0, 0));

			//std::cout << "enemigo creado" << std::endl;
		}

        //si la posicion del jugador es menor que el tunel2 en la mitad
		if(posJugador < nodoTunel2->getPosition().x+65)
		{
			nodoTunel1->setPosition(nodoTunel2->getPosition());
			nodoTunel2->setPosition(nodoTunel1->getPosition()+Ogre::Vector3(-130, 0, 0));
		}
		//de lo contrario, si la posicion del jugador es mayor que la del tunel1 a la mitad
		else if(posJugador > nodoTunel1->getPosition().x+65)
		{
			nodoTunel2->setPosition(nodoTunel1->getPosition());
			nodoTunel1->setPosition(nodoTunel2->getPosition()+Ogre::Vector3(130, 0, 0));
		}
	}

}

bool Juego::frameStarted(const Ogre::FrameEvent &evt)
{
    //capturamos la entrada del usuario
	teclado->capture();
	mouse->capture();

    //si no estamos en pausa
	if(!bPausa)
	{
	    //actualizamos el estado de los enemigos
		actualizarEnemigos(evt.timeSinceLastFrame);
		/**
		checar distEnemigo, posible mal uso
		*/
		//actualizamos el personaje del jugador
		jugador->actualizar(enemigoCercano, distEnemigo, evt.timeSinceLastFrame);
		//actualizamos el tunel, generamos objetos, generamos enemigos (si es necesario)
		actualizarEscenario();
		//si es necesario, actualizamos la GUI
		if(jugador->necesitaActualizarGUI())
			actualizarGUI();
	}

	textoTitulo->setCaption(Ogre::String("Informacion"));
	textoFps->setCaption(Ogre::String("FPS: ") + Ogre::StringConverter::toString(ventana->getLastFPS()));

	return !bSalir;
}

bool Juego::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	return !bSalir;
}

bool Juego::frameEnded(const Ogre::FrameEvent &evt)
{
	return !bSalir;
}

void Juego::windowClosed(Ogre::RenderWindow* rw)
{
	raiz->queueEndRendering();
}

bool Juego::keyPressed(const OIS::KeyEvent &arg)
{
	if(arg.key == OIS::KC_ESCAPE)
	{
		if(!bTeclaESC && !widgetMenuPrincipal->getVisible())
		{
			if(widgetMenuJuego->getVisible())
				widgetMenuJuego->setVisible(false);
			else
				widgetMenuJuego->setVisible(true);

			bPausa = !bPausa;
			bTeclaESC = true;
		}
	}

	MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text);
	return true;
}

bool Juego::keyReleased(const OIS::KeyEvent &arg)
{
	if(arg.key == OIS::KC_ESCAPE)
	{
		if(bTeclaESC)
			bTeclaESC = false;
	}

	MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
	return true;
}

bool Juego::mouseMoved(const OIS::MouseEvent &arg)
{
	MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	return true;
}

bool Juego::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}

bool Juego::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}
