////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Jugador.h"
#include <iostream>
#include <cmath>

Jugador::Jugador(Ogre::SceneManager* gestEscena, Ogre::Camera* camera, OIS::Keyboard* oisTeclado) : Personaje(gestEscena, "sinbad", "Sinbad.mesh", 1, 125, 17, 8, 7, 16.5f), camara(camera), temporizadorSubirNivel(0), teclado(oisTeclado), bTeclaD(false), bTeclaA(false), bTeclaW(false), bTeclaS(false), bTeclaEspacio(false), bInterpolando(false), bSaltando(false), bBajando(false), bNecesitaActualizarGUI(true), bSubiendoNivel(false), _exp(0), _expMaxima(30)
{
	++contador;

	temporizadorSubirNivel = new Ogre::Timer();

	nodoJugador = gestorEscena->getSceneNode("juego")->createChildSceneNode("nodoJugador");
	nodo = nodoJugador->createChildSceneNode("nodoSinbad");
	nodoMesh = nodo->createChildSceneNode();
	nodoMesh->attachObject(entidad);
	nodoCaja = nodo->createChildSceneNode();
	
	particulaSangre = gestorEscena->createParticleSystem("sangre" + Ogre::StringConverter::toString(contador), "sangre");
	nodoParticulaSangre = nodoMesh->createChildSceneNode();
	nodoParticulaSangre->setPosition(0, 3, 0);

	particulaSubirNivel = gestorEscena->createParticleSystem("particulaSubirNivel", "subirNivel");
	nodoParticulaSubirNivel = nodoMesh->createChildSceneNode();

	espada1 = gestorEscena->createEntity("Sword1", "Sword.mesh");
	espada2 = gestorEscena->createEntity("Sword2", "Sword.mesh");
	entidad->attachObjectToBone("Handle.L", espada1);
	entidad->attachObjectToBone("Handle.R", espada2);

	altura = entidad->getBoundingBox().getSize().y * (1.0f - Ogre::MeshManager::getSingleton().getBoundsPaddingFactor());
	nodoMesh->setPosition(0, altura/2, 0);

	actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO), altura, 3.0f);
	mostrarCajaColisiones(true);

	nodoCamara = nodoJugador->createChildSceneNode("nodoCamara");
	nodoCamara->attachObject(camara);
	camara->setPosition(0, 15, 50);
	camara->lookAt(nodo->getPosition());
	nodoCamara->setPosition(9, 0, 0);

	camTiempoActual = 0.0f;
	camTiempoFinal = 0.7f;
	camTiempoInterp = 0.0f;

	animCorrerInf = entidad->getAnimationState("RunBase");
	animCorrerSup = entidad->getAnimationState("RunTop");
	animIdleInf = entidad->getAnimationState("IdleBase");
	animIdleSup = entidad->getAnimationState("IdleTop");
	animSaltarPrincipio = entidad->getAnimationState("JumpStart");
	animSaltarPrincipio->setLoop(false);
	animSaltarFinal = entidad->getAnimationState("JumpEnd");
	animSaltarFinal->setLoop(false);
	animSaltarBucle = entidad->getAnimationState("JumpLoop");
	mapAnimAtacar[0] = entidad->getAnimationState("SliceHorizontal");
	mapAnimAtacar[1] = entidad->getAnimationState("SliceVertical");

	setDireccion(ADELANTE);
	setIdle(true);
}

Jugador::~Jugador()
{
	if(temporizadorSubirNivel)
		delete temporizadorSubirNivel;

	nodoMesh->detachAllObjects();
	gestorEscena->destroyEntity(entidad);
	nodoParticulaSangre->detachAllObjects();
	nodoParticulaSubirNivel->detachAllObjects();
	gestorEscena->destroyParticleSystem(particulaSangre);
	gestorEscena->destroyParticleSystem(particulaSubirNivel);
	gestorEscena->destroySceneNode(nodoParticulaSangre);
	gestorEscena->destroySceneNode(nodoParticulaSubirNivel);
	gestorEscena->destroySceneNode(nodoMesh);
}

void Jugador::setDireccion(direccion dir)
{
	_direccion = dir;

	/*if(_direccion == ADELANTE)
		nodo->setDirection(Ogre::Vector3::UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
	else
		nodo->setDirection(Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);*/

	if(_direccion == ADELANTE)
		nodoMesh->setDirection(Ogre::Vector3::UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
	else
		nodoMesh->setDirection(Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
}

void Jugador::setCorriendo(bool correr)
{
	bCorriendo = correr;
	animCorrerInf->setEnabled(correr);
	animCorrerSup->setEnabled(correr);
	if(!correr)
	{
		animCorrerInf->setTimePosition(0);
		animCorrerSup->setTimePosition(0);
	}
}

void Jugador::setIdle(bool idle)
{
	bIdle = idle;
	animIdleInf->setEnabled(idle);
	animIdleSup->setEnabled(idle);
	if(!idle)
	{
		animIdleInf->setTimePosition(0);
		animIdleSup->setTimePosition(0);
	}
}

void Jugador::subirNivel()
{
	std::cout << "subiendo nivel" << std::endl;

	nodoParticulaSubirNivel->attachObject(particulaSubirNivel);
	temporizadorSubirNivel->reset();
	bSubiendoNivel = true;

	while(_exp >= _expMaxima)
	{
		++_nivel;
		_exp -= _expMaxima;
		_expMaxima += _expMaxima/2;
		_vidaMaxima += _vidaMaxima/8;
		_ptsAtaque += static_cast<int>(sqrt(static_cast<float>(_ptsAtaque))) + 1;
		_ptsDefensa += static_cast<int>(sqrt(static_cast<float>(_ptsDefensa))) - 1;
	}

	_vida = _vidaMaxima;
}

void Jugador::saltar(Ogre::Real tiempoUltimoFrame)
{
	if(!estaSaltando())
	{
		bSaltando = true;
		if(!animSaltarPrincipio->getEnabled())
			animSaltarPrincipio->setEnabled(true);

		if(estaCorriendo())
			setCorriendo(false);
		if(estaIdle())
			setIdle(false);
	}

	if(animSaltarPrincipio->hasEnded() && animSaltarPrincipio->getEnabled())
	{
		animSaltarPrincipio->setEnabled(false);
		animSaltarBucle->setEnabled(true);
	}
	else if(animSaltarBucle->getEnabled())
	{
		if(nodo->getPosition().y < 10 && bBajando == false)
		{
			nodo->translate(_velocidad * Ogre::Vector3(0, 1, 0) * tiempoUltimoFrame, Ogre::Node::TS_PARENT);
		}
		else if(nodo->getPosition().y > 0)
		{
			if(!bBajando)
				bBajando = true;

			nodo->translate(_velocidad * Ogre::Vector3(0, -1, 0) * tiempoUltimoFrame, Ogre::Node::TS_PARENT);
			if(nodo->getPosition().y < 0)
			{
				nodo->setPosition(nodo->getPosition().x, 0, nodo->getPosition().z);
				animSaltarBucle->setEnabled(false);
				animSaltarFinal->setEnabled(true);

				bBajando = false;
			}
		}
	}
	else if(animSaltarFinal->getEnabled())
	{
		if(animSaltarFinal->hasEnded())
		{
			animSaltarFinal->setEnabled(false);

			animSaltarPrincipio->setTimePosition(0);
			animSaltarBucle->setTimePosition(0);
			animSaltarFinal->setTimePosition(0);

			bSaltando = false;
		}
	}


	if(animSaltarPrincipio->getEnabled())
		animSaltarPrincipio->addTime(tiempoUltimoFrame);
	else if(animSaltarBucle->getEnabled())
		animSaltarBucle->addTime(tiempoUltimoFrame);
	else if(animSaltarFinal->getEnabled())
		animSaltarFinal->addTime(tiempoUltimoFrame);
}

void Jugador::sangrar()
{
	if(!bSangrando)
	{
		temporizadorSangre->reset();
		nodoParticulaSangre->attachObject(particulaSangre);
		bSangrando = true;
	}
}

void Jugador::atacar(Enemigo* enemigo, float distEnemigo, Ogre::Real tiempoUltimoFrame)
{
	if(animIdleSup->getEnabled())
		animIdleSup->setEnabled(false);
	if(animCorrerSup->getEnabled())
		animCorrerSup->setEnabled(false);

	if(!estaAtacando())
	{
		numeroRand = rand() % 2;
		animAtacar = mapAnimAtacar[numeroRand];
		animAtacar->setEnabled(true);
		animAtacar->setLoop(false);
		bAtacando = true;
	}

	if(animAtacar->getEnabled())
	{
		if(animAtacar->hasEnded())
		{
			animAtacar->setEnabled(false);
			animAtacar->setTimePosition(0);
			bAtacando = false;
			if(enemigo && sqrt(distEnemigo) < getRangoAtaque() && getDireccion() != enemigo->getDireccion())
			{
				enemigo->sangrar();
				enemigo->decrementarVida(getPtsAtaque() - enemigo->getPtsDefensa());
				enemigo->actualizarBarraVida();
			}

			if(estaIdle())
			{
				if(!animIdleSup->getEnabled())
					animIdleSup->setEnabled(true);
				if(!animCorrerSup->getEnabled())
					animCorrerSup->setEnabled(true);
			}
		}
		else
			animAtacar->addTime(tiempoUltimoFrame);
	}
}

void Jugador::actualizar(Enemigo* enemigo, float distEnemigo, Ogre::Real tiempoDesdeUltimoFrame)
{
    vecDir = Ogre::Vector3::ZERO;

	if(bInterpolando)
	{
		camTiempoActual += tiempoDesdeUltimoFrame;
		camTiempoInterp = camTiempoActual / camTiempoFinal;
		if (camTiempoInterp > camTiempoFinal)
		{
			camTiempoInterp = camTiempoFinal;
			camTiempoActual = 0.0f;
			bInterpolando = false;
		}
		nodoCamara->setPosition(camPosInicial + (camPosFinal-camPosInicial) * camTiempoInterp);
	}

	if(teclado->isKeyDown(OIS::KC_D) && !bTeclaA)
	{
		if(bTeclaD)
		{
			if(!estaSaltando())
			{
				if(!estaCorriendo())
					setCorriendo(true);
				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

            vecDir += Ogre::Vector3::UNIT_X;
		}
		else
		{
			if(estaIdle())
			{
				setIdle(false);
			}

			if(getDireccion() != ADELANTE)
			{
				setDireccion(ADELANTE);

				camTiempoActual = 0.0f;
				camPosFinal = Ogre::Vector3(16, 0, 0);
				camPosInicial = Ogre::Vector3(nodoCamara->getPosition().x, 0, 0);
				bInterpolando = true;
			}

			if(!estaCorriendo())
			{
				if(!estaSaltando())
				{
					setCorriendo(true);

					if(animCorrerInf->getEnabled())
						animCorrerInf->addTime(tiempoDesdeUltimoFrame);
					if(animCorrerSup->getEnabled())
						animCorrerSup->addTime(tiempoDesdeUltimoFrame);
				}

				vecDir += Ogre::Vector3::UNIT_X;
			}

			bTeclaD = true;
		}
	}
	else
	{
		if(bTeclaD)
		{
			if(!bTeclaA && !bTeclaW && !bTeclaS)
				setCorriendo(false);

			bTeclaD = false;
		}
	}

	if(teclado->isKeyDown(OIS::KC_A) && !bTeclaD)
	{
		if(bTeclaA)
		{
			if(!estaSaltando())
			{
				if(!estaCorriendo())
					setCorriendo(true);
				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

			vecDir += Ogre::Vector3::NEGATIVE_UNIT_X;
		}
		else
		{
			if(estaIdle())
			{
				setIdle(false);
			}

			if(getDireccion() != ATRAS)
			{
				setDireccion(ATRAS);

				camTiempoActual = 0.0f;
				camPosFinal = Ogre::Vector3(-16, 0, 0);
				camPosInicial = Ogre::Vector3(nodoCamara->getPosition().x, 0, 0);
				bInterpolando = true;
			}

			if(!estaCorriendo())
			{
				if(!estaSaltando())
				{
					setCorriendo(true);

					if(animCorrerInf->getEnabled())
						animCorrerInf->addTime(tiempoDesdeUltimoFrame);
					if(animCorrerSup->getEnabled())
						animCorrerSup->addTime(tiempoDesdeUltimoFrame);
				}

                vecDir += Ogre::Vector3::NEGATIVE_UNIT_X;
			}

			bTeclaA = true;
		}
	}
	else
	{
		if(bTeclaA)
		{
			if(!bTeclaD && !bTeclaW && !bTeclaS)
				setCorriendo(false);

			bTeclaA = false;
		}
	}

	if(teclado->isKeyDown(OIS::KC_W) && nodoJugador->getPosition().z > -20)
	{
		if(bTeclaW)
		{
			if(!estaSaltando() && !bTeclaD && !bTeclaA)
			{
				if(!estaCorriendo())
					setCorriendo(true);

				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

            vecDir += Ogre::Vector3::NEGATIVE_UNIT_Z;
		}
		else
		{
			if(estaIdle())
				setIdle(false);

			if(!estaSaltando() && !bTeclaD && !bTeclaA)
			{
				if(!estaCorriendo())
					setCorriendo(true);

				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

            vecDir += Ogre::Vector3::NEGATIVE_UNIT_Z;

			bTeclaW = true;
		}
	}
	else
	{
		if(bTeclaW)
		{
			if(!bTeclaD && !bTeclaA && !bTeclaS)
				setCorriendo(false);

			bTeclaW = false;
		}
	}

	if(teclado->isKeyDown(OIS::KC_S) && nodoJugador->getPosition().z < 20)
	{
		if(bTeclaS)
		{
			if(!estaSaltando() && !bTeclaD && !bTeclaA)
			{
				if(!estaCorriendo())
					setCorriendo(true);

				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

            vecDir += Ogre::Vector3::UNIT_Z;
		}
		else
		{
			if(estaIdle())
				setIdle(false);

			if(!estaSaltando() && !bTeclaD && !bTeclaA)
			{
				if(!estaCorriendo())
					setCorriendo(true);

				if(animCorrerInf->getEnabled())
					animCorrerInf->addTime(tiempoDesdeUltimoFrame);
				if(animCorrerSup->getEnabled())
					animCorrerSup->addTime(tiempoDesdeUltimoFrame);
			}

            vecDir += Ogre::Vector3::UNIT_Z;

			bTeclaS = true;
		}
	}
	else
	{
		if(bTeclaS)
		{
			if(!bTeclaD && !bTeclaA && !bTeclaW)
				setCorriendo(false);

			bTeclaS = false;
		}
	}

	if(vecDir != Ogre::Vector3::ZERO)
	{
		vecDir.normalise();

		/*actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO));

		nodoJugador->translate(_velocidad * vecDir * tiempoDesdeUltimoFrame, Ogre::Node::TS_PARENT);
		nodoJugador->_update(true, false); 

		if(estaIntersectando(enemigo))
		{
			nodoJugador->translate(_velocidad * -vecDir * tiempoDesdeUltimoFrame, Ogre::Node::TS_PARENT);
			//actualizarCajaColisiones(nodo->_getDerivedPosition());
			actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO));
			std::cout << "jugador vec1 " << getVec1() << std::endl;
			std::cout << "jugador vec2 " << getVec2() << std::endl;
			std::cout << "enemigo vec1 " << enemigo->getVec1() << std::endl;
			std::cout << "enemigo vec2 " << enemigo->getVec2() << std::endl;
		}*/


		//actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO));
		std::cout << "jugador vec1 " << getVec1() << std::endl;
		std::cout << "jugador vec2 " << getVec2() << std::endl;
		std::cout << "enemigo vec1 " << enemigo->getVec1() << std::endl;
		std::cout << "enemigo vec2 " << enemigo->getVec2() << std::endl;

		Ogre::Real t = 1.0f;

		if(enemigo)
			t = detectarColision(enemigo, _velocidad * vecDir * tiempoDesdeUltimoFrame);

		if(t == 1.0f)
			nodoJugador->translate(t * _velocidad * vecDir * tiempoDesdeUltimoFrame, Ogre::Node::TS_PARENT);
		else
			nodoJugador->translate(t * 0.9f * _velocidad * vecDir * tiempoDesdeUltimoFrame, Ogre::Node::TS_PARENT);

		std::cout << "t " << t << std::endl;
		
		nodoJugador->_update(true, false);

		actualizarCajaColisiones(nodo->convertLocalToWorldPosition(Ogre::Vector3::ZERO));
	}

	if(teclado->isKeyDown(OIS::KC_X))
	{
		atacar(enemigo, distEnemigo, tiempoDesdeUltimoFrame);
	}
	else
	{
		if(estaAtacando())
			atacar(enemigo, distEnemigo, tiempoDesdeUltimoFrame);
	}

	if(teclado->isKeyDown(OIS::KC_SPACE))
	{
		saltar(tiempoDesdeUltimoFrame);
	}
	else
	{
		if(estaSaltando())
			saltar(tiempoDesdeUltimoFrame);
	}

	if(!estaCorriendo() && !estaSaltando())
	{
		if(!estaIdle())
			setIdle(true);

		if(animIdleInf->getEnabled())
			animIdleInf->addTime(tiempoDesdeUltimoFrame);
		if(animIdleSup->getEnabled())
			animIdleSup->addTime(tiempoDesdeUltimoFrame);
	}

	if(estaSangrando() && temporizadorSangre->getMilliseconds() > 300)
	{
		bSangrando = false;
		nodoParticulaSangre->detachObject(particulaSangre);
	}

	if(estaSubiendoNivel() && temporizadorSubirNivel->getMilliseconds() > 5000)
	{
		bSubiendoNivel = false;
		nodoParticulaSubirNivel->detachObject(particulaSubirNivel);
	}

	if(getExp() >= getExpMaxima())
	{
		subirNivel();
	}
}
