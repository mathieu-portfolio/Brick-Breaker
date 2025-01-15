#include "V2.h"
#include <iostream>

using namespace std;

// comparaison sur des flottants... traitement sp cial

bool operator == (const V2& a, const V2& b)
{
	V2 t = a - b;
	float epsilon = 0.001f;
	return t.norm() < epsilon;
}
bool operator != (const V2& a, const V2& b) { return !(a == b); }
bool operator == (const V2& a, int b) { return a == V2(b, b); }
bool operator != (const V2& a, int b) { return !(a == b); }

// red finition des op rateurs standards

V2 operator + (const V2& a, const V2& b) { return V2(a.x + b.x, a.y + b.y); }
V2 operator - (const V2& a, const V2& b) { return V2(a.x - b.x, a.y - b.y); }
V2 operator + (const V2& a, float     b) { return a + V2(b, b); }
V2 operator - (const V2& a, float     b) { return a - V2(b, b); }
V2 operator * (float     a, const V2& b) { return V2(a * b.x, a * b.y); }
V2 operator * (const V2& a, float     b) { return V2(a.x * b, a.y * b); }
V2 operator / (const V2& a, float     b) { return V2(a.x / b, a.y / b); }

V2 operator += (V2& a, const V2& b) { return V2(a.x += b.x, a.y += b.y); }
V2 operator += (V2& a, float     b) { return a += V2(b, b); }
V2 operator -= (V2& a, const V2& b) { return V2(a.x -= b.x, a.y -= b.y); }
V2 operator -= (V2& a, float	 b) { return a -= V2(b, b); }
V2 operator *= (V2& a, const V2& b) { return V2(a.x *= b.x, a.y *= b.y); }
V2 operator *= (V2& a, float     b) { return a *= V2(b, b); }
V2 operator /= (V2& a, const V2& b) { return V2(a.x /= b.x, a.y /= b.y); }
V2 operator /= (V2& a, float     b) { return a /= V2(b, b); }

V2 operator - (const V2& a) { return V2() - a; }


// produit scalaire
float operator * (const V2& a, const V2& b) { return a.x * b.x + a.y * b.y; }

//produit vectoriel
float operator ^ (const V2& a, const V2& b) { return a.x * b.y - a.y * b.x; }


ostream& operator << (ostream& os, V2& t) { return os << "(" << t.x << "," << t.y << ")"; }
 /*
int main()
{
	cout << -V2(1, 1) << endl;
}
*/