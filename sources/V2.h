#pragma once

#include <cmath>
#include <iostream>

using namespace std;

/**
 * @class V2
 * @brief Represents a 2D vector with basic vector operations.
 */
class V2
{
public:
	float x, y; ///< X and Y coordinates of the vector.

	/**
	 * @brief Constructs a V2 vector.
	 * @param _x X-coordinate (default: 0).
	 * @param _y Y-coordinate (default: 0).
	 */
	V2(float _x = 0, float _y = 0) : x(_x), y(_y) {}

	/**
	 * @brief Constructs a unit vector from an angle.
	 * @param angle Angle in radians.
	 */
	V2(double angle) : x(cos(angle)), y(sin(angle)) {}

	/**
	 * @brief Computes the norm (magnitude) of the vector.
	 * @return The vector's norm.
	 */
	float norm() { return sqrt(x * x + y * y); }

	/**
	 * @brief Normalizes the vector.
	 * @return The normalized vector.
	 */
	V2 normalize()
	{
		float norme = norm();
		if (norme == 0) return V2();
		x /= norme;
		y /= norme;
		return *this;
	}

	/**
	 * @brief Computes an orthogonal vector.
	 * @return An orthogonal unit vector.
	 */
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

float operator * (const V2& a, const V2& b); ///< Dot product
float operator ^ (const V2& a, const V2& b); ///< Cross product


ostream& operator << (ostream& os, V2& t);