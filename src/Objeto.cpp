////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Objeto.h"

Objeto::Objeto(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh) : gestorEscena(gestEscena), _altura(1.0f), _anchura(1.0f), cajaColision(0)
{
	//gestorEscena = Ogre::Root::getSingleton().getSceneManager("gestorEscena");
	entidad = gestorEscena->createEntity(nombre, nombreMesh);
	entidad->setCastShadows(true);

	vec1 = Ogre::Vector3::ZERO;
	vec2 = Ogre::Vector3::ZERO;

	/*e.x = _anchura / 2;
	e.z = _anchura / 2;
	e.y = _altura / 2;
	pCenter = Ogre::Vector3::ZERO;*/

	cajaDibujada = false;
}

Objeto::~Objeto()
{
	if(cajaDibujada)
		mostrarCajaColisiones(false);

	if(cajaColision)
	{
		gestorEscena->destroyManualObject(cajaColision);
		cajaColision = 0;
	}
}

void Objeto::actualizarCajaColisiones(Ogre::Vector3 posicion)
{
	Ogre::Real anch = _anchura/2;

	vec1 = Ogre::Vector3(posicion.x - anch, posicion.y, posicion.z - anch);
	vec2 = Ogre::Vector3(posicion.x + anch, posicion.y + _altura, posicion.z + anch);

	//pCenter = Ogre::Vector3(posicion.x, posicion.y + _altura/2, posicion.z);
}

void Objeto::actualizarCajaColisiones(Ogre::Vector3 posicion, Ogre::Real altura, Ogre::Real anchura)
{
	_altura = altura;
	_anchura = anchura;

	Ogre::Real anch = _anchura/2;

	vec1 = Ogre::Vector3(posicion.x - anch, posicion.y, posicion.z - anch);
	vec2 = Ogre::Vector3(posicion.x + anch, posicion.y + _altura, posicion.z + anch);

	//pCenter = Ogre::Vector3(posicion.x, posicion.y + _altura/2, posicion.z);

	dibujarCajaColisiones();
}

const bool Objeto::estaIntersectando(Objeto* obj) const
{
	if(this->vec2.x >= obj->getVec1().x && this->vec1.x <= obj->getVec2().x &&
    this->vec2.y >= obj->getVec1().y && this->vec1.y <= obj->getVec2().y &&
    this->vec2.z >= obj->getVec1().z && this->vec1.z <= obj->getVec2().z)
	{
		std::cout << "hay colision" << std::endl;

		return true;
	}

	return false;
}

void Objeto::dibujarCajaColisiones()
{
	if(cajaColision)
	{
		gestorEscena->destroyManualObject(cajaColision);
		cajaColision = 0;
	}

	cajaColision = gestorEscena->createManualObject();
	cajaColision->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

	cajaColision->position(getVertice(0));
	cajaColision->position(getVertice(1));

	cajaColision->position(getVertice(0));
	cajaColision->position(getVertice(3));

	cajaColision->position(getVertice(0));
	cajaColision->position(getVertice(4));

	cajaColision->position(getVertice(1));
	cajaColision->position(getVertice(2));

	cajaColision->position(getVertice(1));
	cajaColision->position(getVertice(5));

	cajaColision->position(getVertice(2));
	cajaColision->position(getVertice(3));

	cajaColision->position(getVertice(2));
	cajaColision->position(getVertice(6));

	cajaColision->position(getVertice(3));
	cajaColision->position(getVertice(7));

	cajaColision->position(getVertice(4));
	cajaColision->position(getVertice(5));

	cajaColision->position(getVertice(4));
	cajaColision->position(getVertice(7));

	cajaColision->position(getVertice(5));
	cajaColision->position(getVertice(6));

	cajaColision->position(getVertice(6));
	cajaColision->position(getVertice(7));

	cajaColision->end();
}

void Objeto::mostrarCajaColisiones(bool mostrar)
{
	if(mostrar && !cajaDibujada)
	{
		nodoCaja->attachObject(cajaColision);
		cajaDibujada = true;
	}
	else
	{
		nodoCaja->detachObject(cajaColision);
		cajaDibujada = false;
	}
}

Ogre::Real Objeto::detectarColision(Objeto* obj, Ogre::Vector3 v)
{
	Ogre::Vector3 tempVec1;
    Ogre::Vector3 tempVec2;

    for(int i = 0; i < 3; i++)
    {
    tempVec1[i] = v[i] > 0 ? this->getVec1()[i] : this->getVec1()[i] + v[i];
    tempVec2[i] = v[i] > 0 ? v[i] + this->getVec2()[i] : this->getVec2()[i] - v[i];
    }

    if(!(tempVec2.x < obj->getVec1().x || tempVec1.x > obj->getVec2().x || tempVec2.y < obj->getVec1().y || tempVec1.y > obj->getVec2().y || tempVec2.x < obj->getVec1().x || tempVec1.x > obj->getVec2().x))
    {

    //entrada
    Ogre::Vector3 entradaInv;
    //salida
    Ogre::Vector3 salidaInv;

    Ogre::Vector3 entrada;

    Ogre::Vector3 salida;

    for(int i = 0; i < 3; i++)
    {
        if (v[i] > 0.0f)
        {
            entradaInv = obj->getVec1()[i] - this->getVec2()[i];
            salidaInv = obj->getVec2()[i] - this->getVec1()[i];
        }
        else
        {
            entradaInv = obj->getVec2()[i] - this->getVec1()[i];
            salidaInv = obj->getVec1()[i] - this->getVec2()[i];
        }

        if (v[i] == 0.0f)
        {
            entrada[i] = -std::numeric_limits<float>::infinity();
            salida[i] = std::numeric_limits<float>::infinity();
        }
        else
        {
            entrada[i] = entradaInv[i] / v[i];
            salida[i] = salidaInv[i] / v[i];
        }
    }


    Ogre::Real tiempoEntrada = std::max(std::max(entrada.x, entrada.y), entrada.z);
    Ogre::Real tiempoSalida = std::max(std::min(salida.x, salida.y), salida.z);

    if (tiempoEntrada > tiempoSalida || entrada.x < 0.0f && entrada.y < 0.0f && entrada.z < 0.0f || entrada.x > 1.0f || entrada.y > 1.0f || entrada.z > 1.0f)
    {
        return 1.0f;
    }
    else // if there was a collision
    {
        // return the time of collision
        return tiempoEntrada;
    }
	}
	else
		return 1.0f;
}
