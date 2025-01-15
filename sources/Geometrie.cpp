#include "Geometrie.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				GEOMETRIE
//
////////////////////////////////////////////////////////////////////////////////////////////////////


//distance a une droite
float Dist(V2 A, V2 B, V2 P)
{
	V2 N = (A - B).orthogonal();
	N.normalize();
	return abs((A - P) * N);
}

//M du meme cote que P par rapport a AB
bool sameSide(V2 A, V2 B, V2 M, V2 P)
{
	float r1 = (A - B) ^ (A - M);
	float r2 = (A - B) ^ (A - P);
	return (r1 * r2 >= 0);
}


//AB coupe CD
bool interSegmentSegment(V2 A, V2 B, V2 C, V2 D)
{
	return (!sameSide(A, B, C, D) && !sameSide(C, D, A, B));
}


//BOULE TRAVERSE CD
bool detectionCollision(V2 C, V2 D, V2 A, V2 B, int r)
{
	V2 T = A - B;
	T.normalize();
	V2 _A = A; // -r * T;
	V2 _B = B; // +r * T;

	return interSegmentSegment(C, D, _A, _B);
}


bool detectionCollision(Segment CD, V2 A, V2 B, int r) 
{ 
	return detectionCollision(CD.A, CD.B, A, B, r); 
}


bool pointIntersectionSegment(V2 A, V2 B, V2 C, V2 D, V2& intersection)
{
	float a = (B.y - A.y) / (B.x - A.x);
	float b = (B.x * A.y - A.x * B.y) / (B.x - A.x);

	float c = (D.y - C.y) / (D.x - C.x);
	float d = (D.x * C.y - C.x * D.y) / (D.x - C.x);

	if ((a == c) && (b != d)) {
		return false;
	}

	if (A.x == B.x) {
		intersection = V2(A.x, c * A.x + d);
		return true;
	}
	if (C.x == D.x) {
		intersection = V2(C.x, a * C.x + b);
		return true;
	}

	float x = (d - b) / (a - c);
	float y = a * x + b;

	intersection = V2(x, y);
	//cout << "point d'intersection : " << intersection << endl;
	return true;
}

bool pointIntersectionSegment(Segment AB, Segment CD, V2& intersection)
{
	return pointIntersectionSegment(AB.A, AB.B, CD.A, CD.B, intersection);
}


bool pointsIntersectionCercles(V2 A, V2 B, float r, float R, V2& P, V2& Q)
{
	float a = 2 * (B.x - A.x);
	float b = 2 * (B.y - A.y);
	float c = pow(B.x - A.x, 2) + pow(B.y - A.y, 2) - pow(R, 2) + pow(r, 2);
	float delta = pow(2 * a * c, 2) - 4 * (pow(a, 2) + pow(b, 2)) * (pow(c, 2) - pow(b, 2) * pow(r, 2));

	if (delta < 0) {
		return false;
	}
	if (delta >= 0) {
		P.x = A.x + (2 * a * c - sqrt(delta)) / (2 * (pow(a, 2) + pow(b, 2)));
		Q.x = A.x + (2 * a * c + sqrt(delta)) / (2 * (pow(a, 2) + pow(b, 2)));
	}
	if (b != 0) {
		P.y = A.y + (c - a * (P.x - A.x)) / b;
		Q.y = A.y + (c - a * (Q.x - A.x)) / b;
	}
	else {
		P.y = A.y + b / 2 - sqrt(pow(R, 2) - pow((2 * c - pow(a, 2)) / 2 * a, 2));
		Q.y = A.y + b / 2 + sqrt(pow(R, 2) - pow((2 * c - pow(a, 2)) / 2 * a, 2));
	}
	return true;
}

bool pointsIntersectionCercles(V2 A, V2 B, float r, float R, Segment& PQ)
{
	return pointsIntersectionCercles(A, B, r, R, PQ.A, PQ.B);
}


bool collisionSegment(Segment trajectoire, int rayon, float distance,
	vector<Segment> segments, vector<bool>* segmentActif, V2* normal, V2* intersection)
{
	//position de la bille
	V2 pos = trajectoire.A;
	//stocke le resultat
	bool result = false;

	//parcourt des cotes a la recherche d'une intersection
	int i = 0;
	for (Segment segment : segments) {
		//detection d'une intersection avec un cote actif
		bool collision = detectionCollision(segment, pos, trajectoire.B, rayon);
		if ((segmentActif != nullptr) && (i < (*segmentActif).size())) {
			collision &= (*segmentActif)[i];
		}
		if (collision) {
			V2 point_intersection;
			//verifie l'existence d'une intersection
			if (pointIntersectionSegment(segment, trajectoire, point_intersection)) {
				//compare la distance avec temp et modifie les valeurs des pointeurs
				if ((point_intersection - pos).norm() < distance) {
					//actualisation de temp
					distance = (point_intersection - pos).norm();
					//actulisation du vecteur normal
					*normal = segment.orthogonal();
					*intersection = point_intersection;
				}
			}
			result = true;
		}
		i++;
	}
	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				SEGMENT
//
////////////////////////////////////////////////////////////////////////////////////////////////////


bool operator == (const Segment& a, const Segment& b) { return (a.A == b.A) && (a.B == b.B) ||
																(a.A == b.B) && (a.B == b.A); }
bool operator != (const Segment& a, const Segment& b) { return !(a == b); }

Segment operator + (const Segment& a, const Segment& b) { return Segment(a.A + b.A, a.B + b.B); }
Segment operator - (const Segment& a, const Segment& b) { return Segment(a.A - b.A, a.B - b.B); }
Segment operator * (float          a, const Segment& b) { return Segment(b.A, a * b.B); }
Segment operator * (const Segment& a, float          b) { return Segment(a.A, a.B * b); }
Segment operator / (const Segment& a, float          b) { return Segment(a.A, a.B / b); }

ostream& operator << (ostream& os, Segment& AB) 
{ 
	return os << "[(" << AB.A.x << ", " << AB.A.y << "), (" << AB.B.x << ", " << AB.B.y << ")]"; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				RECTANGLE
//
////////////////////////////////////////////////////////////////////////////////////////////////////


ostream& operator << (ostream& os, Rect& r) 
{ 
	return os << Segment(r.A, r.C); 
}


/*
int main() {
	V2 M = V2(50, 50);
	int rayon = 15;
	Rect rect = Rect(V2(), V2(600, 800));
	for (Segment cote : rect.cotes)
		cout << cote << endl;
	rect.debug_collision(M, rayon, true);
	M = V2(-50, 50);
	rect.debug_collision(M, rayon, true);
	M = V2(50, -50);
	rect.debug_collision(M, rayon, true);
	M = V2(1000, 50);
	rect.debug_collision(M, rayon, true);
	M = V2(50, 1000);
	rect.debug_collision(M, rayon, true);
}
*/