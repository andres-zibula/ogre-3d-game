////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Enemigo.h"

Enemigo::Enemigo(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh, int nivel, int vida, int ptsAtaque, int ptsDefensa, int rangoAtaque, float velocidad, const Ogre::Vector3& escala) : Personaje(gestEscena, nombre + Ogre::StringConverter::toString(++contador), nombreMesh, nivel, vida, ptsAtaque, ptsDefensa, rangoAtaque, velocidad), tipoEnemigo(nombre)
{
	std::cout << "tipo " << tipoEnemigo << " nombre " << nombre << std::endl;

	//segun el nivel del enemigo establecemos sus atributos
	_vidaMaxima += _nivel* (_vidaMaxima/5);
	_vida = _vidaMaxima;
	_ptsAtaque += _nivel * (_ptsAtaque/6) + 2;
	_ptsDefensa += _nivel * (_ptsDefensa/5);

	//creamos el nodo
	nodo = gestorEscena->getSceneNode("enemigos")->createChildSceneNode(nombre + Ogre::StringConverter::toString(contador));
	nodoMesh = nodo->createChildSceneNode();
	nodoMesh->attachObject(entidad);
	nodo->setPosition(0, 0, 0);
	//escalamos el mesh
	nodoMesh->setScale(escala);
	nodoCaja = nodo->createChildSceneNode();

	particulaSangre = gestorEscena->createParticleSystem("sangre" + Ogre::StringConverter::toString(contador), "sangre");
	nodoParticulaSangre = nodo->createChildSceneNode();
	nodoParticulaSangre->setPosition(0, 5, 0);
	
	bbsBarraVida = gestorEscena->createBillboardSet(Ogre::String("bbBarraVida") + Ogre::StringConverter::toString(contador));
    bbsBarraVida->setMaterialName("barraVida");
	bbsBarraVida->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	bbBarraVida = bbsBarraVida->createBillboard(Ogre::Vector3::ZERO);
    bbBarraVida->setDimensions(3, 1);
    bbBarraVida->setTexcoordRect(0.0f, 0.0f, 0.5f, 1.0f);
    nodoBarraVida = nodo->createChildSceneNode(Ogre::String("nodoBarraVida") + Ogre::StringConverter::toString(contador));
    nodoBarraVida->attachObject(bbsBarraVida);
	nodoBarraVida->setPosition(Ogre::Vector3(0, 10, 0));

	barraTexto = new Ogre::MovableText("movableText" + Ogre::StringConverter::toString(contador), tipoEnemigo + Ogre::String(" Nivel:") + Ogre::StringConverter::toString(_nivel) + Ogre::String(" Ataque:") + Ogre::StringConverter::toString(_ptsAtaque) + Ogre::String(" Defensa:") + Ogre::StringConverter::toString(_ptsDefensa), "ubuntu", 0.7f);
	barraTexto->setTextAlignment(Ogre::MovableText::H_CENTER, Ogre::MovableText::V_ABOVE);
	nodoTexto = nodo->createChildSceneNode();
	nodoTexto->attachObject(barraTexto);
	nodoTexto->setPosition(Ogre::Vector3(0, 10, 0));

	if(tipoEnemigo == "ninja")
	{
		nodoBarraVida->translate(0, 0, 0);
		nodoTexto->translate(2, 2, 0);
		animCorrer = entidad->getAnimationState("Walk");
		animIdle = entidad->getAnimationState("Idle1");
		animAtacar = entidad->getAnimationState("Attack3");
		animMorir = entidad->getAnimationState("Death1");

		mapAnimAtacar[0] = entidad->getAnimationState("Attack1");
		mapAnimAtacar[1] = entidad->getAnimationState("Attack3");
		mapAnimAtacar[2] = entidad->getAnimationState("SideKick");
		mapAnimAtacar[3] = entidad->getAnimationState("Spin");

		actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO), 9.0f, 3.0f);
	}
	else if(tipoEnemigo == "demonio")
	{
		nodoBarraVida->translate(0, 5, 0);
		nodoTexto->translate(2, 7, 0);
		animCorrer = entidad->getAnimationState("run");
		animIdle = entidad->getAnimationState("idle");
		animMorir = entidad->getAnimationState("die");

		actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO), 10.0f, 4.0f);
	}
	else if(tipoEnemigo == "golem")
	{
		nodoBarraVida->translate(0, 7, 0);
		nodoTexto->translate(2, 9, 0);
		animCorrer = entidad->getAnimationState("run");
		animIdle = entidad->getAnimationState("idle");
		animMorir = entidad->getAnimationState("die");

		actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO), 12.0f, 5.0f);
	}
	else if(tipoEnemigo == "goblin")
	{
		nodoBarraVida->translate(0, 0, 0);
		nodoTexto->translate(2, 2, 0);
		animCorrer = entidad->getAnimationState("run");
		animIdle = entidad->getAnimationState("idle");
		animMorir = entidad->getAnimationState("die");

		actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO), 4.0f, 2.0f);
	}

	mostrarCajaColisiones(true);

	animIdle->setEnabled(true);
	animIdle->setLoop(true);
	animMorir->setLoop(false);
	setDireccion(ATRAS);
	setIdle(true);
}

Enemigo::~Enemigo(void)
{
	nodoMesh->detachAllObjects();
	gestorEscena->destroyEntity(entidad);
	nodoBarraVida->detachAllObjects();
	gestorEscena->destroySceneNode(nodoBarraVida);
	gestorEscena->destroyBillboardSet(bbsBarraVida);
	nodoParticulaSangre->detachAllObjects();
	gestorEscena->destroySceneNode(nodoParticulaSangre);
	gestorEscena->destroyParticleSystem(particulaSangre);


	std::cout << "enemigo muerto: destructor enemigo" << std::endl;
	nodoTexto->detachAllObjects();

	if(barraTexto)
		delete barraTexto;
}

void Enemigo::setDireccion(direccion dir)
{
	_direccion = dir;

	if(tipoEnemigo == "ninja")
	{
		if(_direccion == ADELANTE)
			nodoMesh->setDirection(Ogre::Vector3::UNIT_X, Ogre::Node::TS_PARENT);
		else
			nodoMesh->setDirection(Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Node::TS_PARENT);
	}
	else
	{
		if(_direccion == ADELANTE)
			nodoMesh->setDirection(Ogre::Vector3::UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
		else
			nodoMesh->setDirection(Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
	}
}

void Enemigo::setCorriendo(bool correr)
{
	animCorrer->setEnabled(correr);
	bCorriendo = correr;

	if(!correr)
        animCorrer->setTimePosition(0);
}

void Enemigo::setIdle(bool idle)
{
	animIdle->setEnabled(idle);
	bIdle = idle;

	if(!idle)
        animIdle->setTimePosition(0);
}

void Enemigo::sangrar()
{
	if(!bSangrando)
	{
		temporizadorSangre->reset();
		nodoParticulaSangre->attachObject(particulaSangre);
		bSangrando = true;
	}
}

void Enemigo::atacar(Jugador* jugador, Ogre::Real &tiempoUltimoFrame)
{
	if(estaIdle())
		setIdle(false);
	if(estaCorriendo())
		setCorriendo(false);

	if(bAtacando == false)
	{
		if (tipoEnemigo == "ninja")
		{
			numeroRand = rand() % 4;
			animAtacar = mapAnimAtacar[numeroRand];
			animAtacar->setEnabled(true);
			animAtacar->setLoop(false);
		}
		else
		{
			animAtacar = entidad->getAnimationState("attack");
			animAtacar->setEnabled(true);
			animAtacar->setLoop(false);
		}

		bAtacando = true;
	}

	if(animAtacar->getEnabled())
	{
		if(animAtacar->hasEnded())
		{
			animAtacar->setEnabled(false);
			animAtacar->setTimePosition(0);
			bAtacando = false;
			jugador->sangrar();
			jugador->decrementarVida(getPtsAtaque() - jugador->getPtsDefensa());
			jugador->necesitaActualizarGUI(true);
		}
		else
			animAtacar->addTime(tiempoUltimoFrame);
	}
}

void Enemigo::actualizar(Jugador* jugador, Ogre::Real &tiempoUltimoFrame)
{
	distJugador = nodo->getPosition().distance(jugador->getNodoJugador()->getPosition());

	if(distJugador < 20 && tieneVida() && jugador->tieneVida())
	{
		if((nodo->getPosition().x > jugador->getNodoJugador()->getPosition().x))
		{
			if(getDireccion() != Personaje::ATRAS)
				setDireccion(Personaje::ATRAS);

			if(distJugador > getRangoAtaque())
			{
				if(estaAtacando())
				{
					animAtacar->setEnabled(false);
					animAtacar->setTimePosition(0);
					bAtacando = false;
				}
				if(estaIdle())
					setIdle(false);
				if(!estaCorriendo())
					setCorriendo(true);

				animCorrer->addTime(tiempoUltimoFrame);

                vecDir = (jugador->getNodoJugador()->getPosition() + Ogre::Vector3(getRangoAtaque()-0.01f, 0, 0)) - nodo->getPosition();
                vecDir.normalise();
                nodo->translate(_velocidad * vecDir * tiempoUltimoFrame, Ogre::Node::TS_PARENT);
			}
			else
			{
				atacar(jugador, tiempoUltimoFrame);
			}
		}
		else if((nodo->getPosition().x < jugador->getNodoJugador()->getPosition().x))
		{
			if(getDireccion() != Personaje::ADELANTE)
			{
				setDireccion(Personaje::ADELANTE);
			}

			if(distJugador > getRangoAtaque())
			{
				if(estaAtacando())
				{
					animAtacar->setEnabled(false);
					animAtacar->setTimePosition(0);
					bAtacando = false;
				}
				if(estaIdle())
					setIdle(false);
				if(!estaCorriendo())
					setCorriendo(true);

				animCorrer->addTime(tiempoUltimoFrame);

                vecDir = (jugador->getNodoJugador()->getPosition() + Ogre::Vector3(-getRangoAtaque()+0.01f, 0, 0)) - nodo->getPosition();
                vecDir.normalise();
                nodo->translate(_velocidad * vecDir * tiempoUltimoFrame, Ogre::Node::TS_PARENT);
			}
			else
			{
				atacar(jugador, tiempoUltimoFrame);
			}
		}
	}
	else if(tieneVida())
	{
		if(estaCorriendo())
			setCorriendo(false);
		if(!estaIdle())
			setIdle(true);
	}
	else
	{
		if(estaCorriendo())
			setCorriendo(false);
		if(estaIdle())
			setIdle(false);
		if(estaAtacando())
			animAtacar->setEnabled(false);

		if(!animMorir->getEnabled())
		{
			animMorir->setEnabled(true);
			bbsBarraVida->setVisible(false);
			barraTexto->setVisible(false);
			jugador->aumentarExp(_nivel*(_vidaMaxima/6));
			jugador->necesitaActualizarGUI(true);
		}

		if(animMorir->getEnabled())
			animMorir->addTime(tiempoUltimoFrame);

		if(animMorir->hasEnded())
		{
			bVivo = false;
		}
	}

	if(estaIdle())
		animIdle->addTime(tiempoUltimoFrame);

	if(estaSangrando() && temporizadorSangre->getMilliseconds() > 300)
	{
		bSangrando = false;
		nodoParticulaSangre->detachAllObjects();
	}

	//actualizamos el nodo para actualizar la caja
	nodo->_update(true, false);

	//actualizamos los vectores de la caja de colisiones
	//actualizarCajaColisiones(nodo->_getDerivedPosition());
	actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO));
}

void Enemigo::actualizarTexto()
{
	barraTexto->setCaption(tipoEnemigo + Ogre::String(" Nivel:") + Ogre::StringConverter::toString(_nivel) + Ogre::String(" Ataque:") + Ogre::StringConverter::toString(_ptsAtaque) + Ogre::String(" Defensa:") + Ogre::StringConverter::toString(_ptsDefensa));
}

void Enemigo::actualizarBarraVida()
{
	Ogre::Real ratio = (Ogre::Real)_vida / (Ogre::Real)_vidaMaxima;

    if (ratio < 0.0f)
        ratio = 0.0f;

    bbBarraVida->setTexcoordRect((1.0f - ratio) / 2.0f, 0.0f, 0.5f + (1.0f - ratio) / 2.0f, 1.0f);
}
