#pragma once

#include "Geometrie.h"
#include <string>

#define LARGEUR_BRIQUE 609 / 7
#define HAUTEUR_BRIQUE 620 / 8
#define LARGEUR_BONUS LARGEUR_BRIQUE / 3
#define HAUTEUR_BONUS HAUTEUR_BRIQUE / 3

#define LARGEUR_ECRAN LARGEUR_BRIQUE * 7
#define HAUTEUR_ECRAN HAUTEUR_BRIQUE * 10
#define HAUTEUR_LANCEMENT HAUTEUR_BRIQUE

#define BALLRAD 10
#define BALLINIT V2(300, BALLRAD + 10)
#define KEYSIZE 40
#define APOS V2 (70, HAUTEUR_ECRAN - 200)

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				ELEMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class Element
{
public:
	Element(V2 _pos = V2(), V2 _size = V2()) : pos(_pos), size(_size) {}

	V2 pos, size;
	int idTex;
	string texture;

	void descend();
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BRIQUE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class Brique : public Element
{
public:
	Brique(V2 _pos = V2(), V2 _size = V2(LARGEUR_BRIQUE, HAUTEUR_BRIQUE), 
		int res = 1, int _points = 1, bool _intact = true) :
		Element(_pos, _size), resistance(res), points(_points), intact(_intact) 
	{
		coteActif = { true, true, true, true };
		//init_cotes();
	}

	Brique(V2 _pos, int res) :
		Brique(_pos, V2(LARGEUR_BRIQUE, HAUTEUR_BRIQUE), res, res, true) {}

	bool intact;
	vector<bool> coteActif;
	int resistance;
	int points;

	bool interBrick(V2 _pos);

	void init_cotes();

	bool collisionBrique(V2 _pos, V2 newPos, int rayon, float distance, 
		V2* n = nullptr, V2* pos = nullptr);
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BILLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class Bille
{
public :
	Bille(V2 _pos = BALLINIT, V2 _speed = V2(), float _rayon = 10,
		double envoi = 0, double rebond = 0) :
		pos(_pos), rayon(_rayon), speed(_speed),
		timer_envoi(envoi), timer_rebond(rebond) {}

	V2 pos, speed;
	float rayon;

	double timer_envoi;
	double timer_rebond;

	void gestionCollisions(V2& newPos, V2& newSpeed, double dt, float distance);
	void gestionRebond(double dt);
};


class Billes : public vector<Bille>
{
public:
	int nbBilles;
	double timer_envoi = 0;

	void push_back(Bille bille);
	void push_back(V2 pos, V2 speed, float rayon);

	void envoi(double dt, int& idx);

	void initialiser();

	Billes()
	{
		initialiser();
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				CLAVIER
//
////////////////////////////////////////////////////////////////////////////////////////////////////


class Touches
{
public:
	Element A, B, C, D, E, F, G, H, I, J,
		K, L, M, N, O, P, Q, R, S, T,
		U, V, W, X, Y, Z, point, tiret, _, supp,
		end;

	vector<Element> liste;

	void initialiser();

	Element getTouch(string text);

	Touches()
	{
		initialiser();
	}
};


class Clavier
{
public :
	void initialiser(V2 pos = APOS, double keySize = KEYSIZE, int size = 10);

	Clavier(V2 pos = APOS, double keySize = KEYSIZE, int size = 10)
	{ 
		initialiser(pos, keySize, size); 
	}

	vector<Element> touches;

	Element A, B, C, D, E, F, G, H, I, J,
			K, L, M, N, O, P, Q, R, S, T,
			U, V, W, X, Y, Z, point, tiret, _, supp,
			end;

	int currentTouch;
	int colonnes;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BONUS
//
////////////////////////////////////////////////////////////////////////////////////////////////////

class Bonus : public Element
{
public :
	Bonus(V2 _pos) :
		Element(_pos, V2(LARGEUR_BONUS, HAUTEUR_BONUS)) {}

	void bonusCaught(V2 _pos, int rayon);

	bool caught = false;
};
