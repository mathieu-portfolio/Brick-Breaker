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

/**
 * @class Element
 * @brief Represents a base game element with a position and size.
 */
class Element
{
public:
	V2 pos, size; ///< Position and size of the element.
	int idTex; ///< Texture ID.
	std::string texture; ///< Texture file name.

	/**
	 * @brief Constructs an Element.
	 */
	Element(V2 _pos = V2(), V2 _size = V2()) : pos(_pos), size(_size) {}

	/**
	 * @brief Moves the element downward.
	 */
	void descend();
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BRIQUE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class Brique
 * @brief Represents a brick in the game.
 */
class Brique : public Element
{
public:
	bool intact; ///< Indicates if the brick is intact.
	vector<bool> coteActif; ///< Active sides of the brick.
	int resistance; ///< Brick resistance level.
	int points; ///< Points awarded when destroyed.

	Brique(V2 _pos = V2(), V2 _size = V2(LARGEUR_BRIQUE, HAUTEUR_BRIQUE), 
		int res = 1, int _points = 1, bool _intact = true) :
		Element(_pos, _size), resistance(res), points(_points), intact(_intact) 
	{
		coteActif = { true, true, true, true };
		//init_cotes();
	}

	/**
	 * @brief Constructs a brick with default dimensions.
	 */
	Brique(V2 _pos, int res);
	
	/**
	 * @brief Checks for brick intersection.
	 */
	bool interBrick(V2 _pos);

	/**
	 * @brief Handles brick collision.
	 */
	bool collisionBrique(V2 _pos, V2 newPos, int rayon, float distance,
												V2* n = nullptr, V2* pos = nullptr);

	void init_cotes();
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BILLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class Bille
 * @brief Represents a ball in the game.
 */
class Bille
{
public :
	V2 pos, speed; ///< Position and speed of the ball.
	float rayon; ///< Ball radius.
	double timer_envoi, timer_rebond; ///< Timers for launch and bounce.

	/**
	 * @brief Constructs a ball.
	 */
	Bille(V2 _pos = BALLINIT, V2 _speed = V2(), float _rayon = 10,
		double envoi = 0, double rebond = 0) :
		pos(_pos), rayon(_rayon), speed(_speed),
		timer_envoi(envoi), timer_rebond(rebond) {}

	/**
	 * @brief Manages ball collisions.
	 */
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

/**
 * @class Bonus
 * @brief Represents a collectible bonus in the game.
 */
class Bonus : public Element
{
public :
	bool caught = false; ///< Indicates if the bonus has been collected.

	/**
	 * @brief Constructs a Bonus.
	 */
	Bonus(V2 _pos) : Element(_pos, V2(LARGEUR_BONUS, HAUTEUR_BONUS)) {}

	/**
	 * @brief Handles bonus collection.
	 */
	void bonusCaught(V2 _pos, int rayon);
};
