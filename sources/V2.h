#pragma once

#include <cmath>
#include <iostream>

using namespace std;

class V2
{
public :
	float x, y;

	V2(float _x = 0, float _y = 0) : 
		x(_x), y(_y) {}

	V2(double angle) :
		x(cos(angle)), y(sin(angle)) {}

	float norm() { return sqrt(x * x + y * y); }

	V2  normalize() 
	{ 
		float norme = norm();  

		if (norme == 0) {
			return V2();
		}

		x /= norme;    
		y /= norme; 

		return *this;
	}

	V2 orthogonal() 
	{ 
		V2 ortho = V2(-y, x);
		ortho.normalize();
		return ortho;
	}
};

// comparaison sur des flottants... traitement sp cial

bool operator == (const V2& a, const V2& b);
bool operator != (const V2& a, const V2& b);
bool operator == (const V2& a, int b);
bool operator != (const V2& a, int b);

// red finition des op rateurs standards

V2 operator + (const V2& a, const V2& b);
V2 operator - (const V2& a, const V2& b);
V2 operator + (const V2& a, float     b);
V2 operator - (const V2& a, float     b);
V2 operator * (float     a, const V2& b);
V2 operator * (const V2& a, float     b);
V2 operator / (const V2& a, float     b);

V2 operator += (V2& a, const V2& b);
V2 operator += (V2& a, float     b);
V2 operator -= (V2& a, const V2& b);
V2 operator -= (V2& a, float	 b);
V2 operator *= (V2& a, const V2& b);
V2 operator *= (V2& a, float     b);
V2 operator /= (V2& a, const V2& b);
V2 operator /= (V2& a, float	 b);

V2 operator - (const V2& a);

// produit scalaire
float operator * (const V2& a, const V2& b);

//produit vectoriel
float operator ^ (const V2& a, const V2& b);


ostream& operator << (ostream& os, V2& t);