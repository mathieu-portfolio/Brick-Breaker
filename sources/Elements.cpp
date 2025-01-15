#include "Elements.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				ELEMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////


void Element::descend()
{
	pos.y -= HAUTEUR_BRIQUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BRIQUE
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//verifier si un point est dans une brique
bool Brique::interBrick(V2 _pos)
{
	//hitbox de la brique
	Rect brickHB = Rect(pos, pos + size);
	return brickHB.interRect(_pos);
}

//verifier si la trajectoire de la balle coupe un cote de la brique
bool Brique::collisionBrique(V2 _pos, V2 newPos, int rayon, float distance, V2* normal, V2* ballPos)
{
	Segment trajectoire = Segment(_pos, newPos);
	Rect briqueHB = Rect(pos, pos + size);
	
	return collisionSegment(trajectoire, rayon, distance, 
		briqueHB.cotes, &coteActif, normal, ballPos);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BILLES
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//surchages de push_back
void Billes::push_back(Bille bille)
{
	vector::push_back(bille);
}

void Billes::push_back(V2 pos, V2 speed, float rayon)
{
	push_back(Bille(pos, speed, rayon));
}


void Billes::initialiser()
{
	nbBilles = 0;

	clear();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				CLAVIER
//
////////////////////////////////////////////////////////////////////////////////////////////////////


void Touches::initialiser()
{
	//alimentation de la liste des touches
	liste = { A, B, C, D, E, F, G, H, I, J,
		K, L, M, N, O, P, Q, R, S, T,
		U, V, W, X, Y, Z, point, tiret, _, supp,
		end };

	//association de chaque touche a une string
	for (int i = 0; i < liste.size(); i++) {
		Element elt = liste[i];
		if (i < 26) {
			elt.texture = (char)('A' + i);
		}
		else if (i == 26) {
			elt.texture = '.';
		}
		else if (i == 27) {
			elt.texture = '-';
		}
		else if (i == 28) {
			elt.texture = ' ';
		}
		else if (i == 29) {
			elt.texture = "supp";
		}
		else {
			elt.texture = "end";
		}
	}
}


Element Touches::getTouch(string text)
{
	for (Element elt : liste) {
		if (elt.texture.compare(text)) {
			return elt;
		}
	}
	return Element();
}


void Clavier::initialiser(V2 pos, double keySize, int size)
{
	//alimentation de la liste des touches
	touches = { A, B, C, D, E, F, G, H, I, J,
				K, L, M, N, O, P, Q, R, S, T,
				U, V, W, X, Y, Z, point, tiret, _, supp, 
				end};

	//association de chaque touche a une string, une taille et une position
	auto itCase = touches.begin();
	itCase->pos = pos;

	int i = 0;
	int j = 0;

	while (itCase != touches.end() - 1) {
		auto nextCase = itCase + 1;

		itCase->size = V2(keySize, keySize);

		if (i < 26) {
			itCase->texture = (char)('A' + i);
		}
		else if (i == 26) {
			itCase->texture = '.';
		}
		else if (i == 27) {
			itCase->texture = '-';
		}
		else if (i == 28) {
			itCase->texture = ' ';
		}
		else {
			itCase->texture = "supp";
			itCase->size = V2(4 * keySize, keySize);
			nextCase->texture = "end";
			nextCase->size = V2(3 * keySize, keySize);
		}

		if (j < size - 1) {
			nextCase->pos = itCase->pos + V2(itCase->size.x, 0);
			j++;
		}
		else {
			nextCase->pos = V2(pos.x, itCase->pos.y - itCase->size.y);
			j = 0;
		}

		itCase++;
		i++;
	}

	currentTouch = 0;
	colonnes = size;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				BONUS
//
////////////////////////////////////////////////////////////////////////////////////////////////////


void Bonus::bonusCaught(V2 _pos, int rayon)
{
	//hitbox du bonus
	Rect bonusHB = Rect(pos, pos + size);
	//hitbox de la balle
	Rect ballHB = Rect(_pos - rayon, _pos + rayon);

	if (ballHB.interRectRect(bonusHB)) {
		caught = true;
	}
}