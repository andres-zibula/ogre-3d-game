////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef OBJETO_H_
#define OBJETO_H_

#pragma once
#include <OGRE/Ogre.h>
#include <algorithm>

class Objeto
{
public:
	Objeto(Ogre::SceneManager* gestEscena, const Ogre::String& nombre, const Ogre::String& nombreMesh);
	virtual ~Objeto();

	//devuelve un puntero a la entidad
	const Ogre::Entity* getEntidad() const {return entidad;}
	//devuelve un puntero al nodo
	Ogre::SceneNode* getNodo() const {return nodo;}
	//devuelve un puntero al nodo nodoMesh
	Ogre::SceneNode* getNodoMesh() const {return nodoMesh;}
	//devuelve la posicion del nodo
	const Ogre::Vector3& getPosicion() const {return nodo->getPosition();}
	//devuelve el vector 1 de la caja de colisiones
	Ogre::Vector3& getVec1() {return vec1;}
	//devuelve el vector 2 de la caja de colisiones
	Ogre::Vector3& getVec2() {return vec2;}
	//ASDASDASDASDASDSDA
	const Ogre::Real getVecMinCol(Ogre::Vector3 v);
	//devuelve el vertice de la caja
	const Ogre::Vector3 getVertice(unsigned short int vertice) const
	{
		switch(vertice)
		{
		case 0:
			return Ogre::Vector3(vec1.x, vec1.y, vec2.z);
		case 1:
			return Ogre::Vector3(vec2.x, vec1.y, vec2.z);
		case 2:
			return vec2;
		case 3:
			return Ogre::Vector3(vec1.x, vec2.y, vec2.z);
		case 4:
			return vec1;
		case 5:
			return Ogre::Vector3(vec2.x, vec1.y, vec1.z);
		case 6:
			return Ogre::Vector3(vec2.x, vec2.y, vec1.z);
		case 7:
			return Ogre::Vector3(vec1.x, vec2.y, vec1.z);
		default:
			return Ogre::Vector3::ZERO;
		}
	}

	//devuelve true si los objetos se intersectan
	const bool estaIntersectando(Objeto* objeto) const;

	//actualizamos las esquinas dada su nueva posicion
	void actualizarCajaColisiones(Ogre::Vector3 posicion);
	//actualizamos las esquinas dada la posicion, altura y anchura
	void actualizarCajaColisiones(Ogre::Vector3 posicion, Ogre::Real altura, Ogre::Real anchura);
	//cambia la posicion del nodo
	void setPosicion(Ogre::Vector3 posicion) {nodo->setPosition(posicion);}
	//cambia la posicion del nodo
	void setPosicion(float x, float y, float z) {nodo->setPosition(x, y, z);}
	//dibuja la caja de colisiones
	void dibujarCajaColisiones();
	//muestra la caja
	void mostrarCajaColisiones(bool mostrar);

	Ogre::Real detectarColision(Objeto* obj, Ogre::Vector3 v);

	/*const Ogre::Real min(int i) const {return this->vec1[i];}

	const Ogre::Real max(int i) const {return this->vec2[i];}*/



protected:
	Ogre::SceneManager* gestorEscena;
	Ogre::Entity* entidad;
	Ogre::SceneNode* nodo;
	Ogre::SceneNode* nodoMesh;
	Ogre::SceneNode* nodoCaja;

private:
	/*
	   7----------6
	  /|         /|
     / |        / |
	3----------2  |
	|  |       |  |
	|  4-------|--5
	| /        | /
	|/         |/
	0----------1
	*/
	Ogre::Vector3 vec1; //vertice 4
	Ogre::Vector3 vec2; //vertice 2

	//cambiar esto

	/*Ogre::Vector3 e;
	Ogre::Vector3 pCenter;*/

	

	Ogre::Real _altura;
	Ogre::Real _anchura;

	Ogre::ManualObject* cajaColision;
	bool cajaDibujada;
};

#endif /* OBJETO_H_ */
