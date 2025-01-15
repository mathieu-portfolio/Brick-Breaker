#pragma once

#include "V2.h"
#include <vector>

struct Segment;

//distance a une droite
float dist(V2 A, V2 B, V2 P);

//M du meme cote que P par rapport a AB
bool sameSide(V2 A, V2 B, V2 M, V2 P);

//AB coupe CD
bool interSegmentSegment(V2 A, V2 B, V2 C, V2 D);

//BOULE TRAVERSE CD
bool detectionCollision(V2 C, V2 D, V2 A, V2 B, int r);
bool detectionCollision(Segment CD, V2 A, V2 B, int r);

//point d'intersection entre deux droites
bool pointIntersectionSegment(V2 A, V2 B, V2 C, V2 D, V2& intersection);
bool pointIntersectionSegment(Segment AB, Segment CD, V2& intersection);

//points d'intersection entre deux cercles
bool pointsIntersectionCercles(V2 A, V2 B, float r, float R, Segment& intersection);

//
bool collisionSegment(Segment trajectoire, int rayon, float distance, vector<Segment> segments, 
	vector<bool>* segmentActif = nullptr, V2* normal = nullptr, V2* intersection = nullptr);


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				SEGMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////


struct Segment
{
	V2 A, B;

	Segment(V2 _A = V2(), V2 _B = V2()) :
		A(_A), B(_B) {}

	V2 centre() { return (A + B) / 2; }

	V2 orthogonal(float distance = 1) 
	{ 
		V2 ortho = (B - A).orthogonal();
		return distance * ortho; 
	}
};

bool operator == (const Segment& a, const Segment& b);
bool operator != (const Segment& a, const Segment& b);

Segment operator + (const Segment& a, const Segment& b);
Segment operator - (const Segment& a, const Segment& b);
Segment operator * (float          a, const Segment& b);
Segment operator * (const Segment& a, float          b);
Segment operator / (const Segment& a, float          b);


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				RECTANGLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////


struct Rect
{
	float xmin, xmax, ymin, ymax;
	V2 A = V2(xmin, ymin);
	V2 B = V2(xmin, ymax);
	V2 C = V2(xmax, ymax);
	V2 D = V2(xmax, ymin);
	vector<V2> points = { A, B, C, D };
	Segment AB = Segment(A, B);
	Segment BC = Segment(B, C);
	Segment CD = Segment(C, D);
	Segment DA = Segment(D, A);
	vector<Segment> cotes = { AB, BC, CD, DA };

	Rect(float _xmin, float _ymin, float _xmax, float _ymax) : xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax) {}
	Rect(V2 _A, V2 _C) : Rect(_A.x, _A.y, _C.x, _C.y) {}
	Rect(V2 O, float c) : Rect(O, O + V2(c, c)) {}

	bool interRect(V2 point)
	{
		if (point.x < xmin) return false;
		if (point.y < ymin) return false;
		if (point.x > xmax) return false;
		if (point.y > ymax) return false;

		return true;
	}

	bool interRectRect(Rect rect)
	{
		if (rect.ymax < ymin) return false;
		if (rect.ymin > ymax) return false;
		if (rect.xmin > xmax) return false;
		if (rect.xmax < xmin) return false;

		return true;
	}


	V2 collision0(V2 pos, float rayon)
	{
		if (pos.x - rayon < 0) return V2(1, 0);
		if (pos.y + rayon > 800) return V2(0, -1);
		if (pos.x + rayon > 600) return V2(-1, 0);

		return V2();
	}
};


ostream& operator << (ostream& os, Segment& AB);
ostream& operator << (ostream& os, Rect& r);