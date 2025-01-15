#pragma once


#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include "V2.h"
 


struct Color
{
	float R, G, B, A;

	Color(float r, float g, float b)          { R = r; G = g; B = b; A = 1; }
	Color(float r, float g, float b, float a) { R = r; G = g; B = b; A = a; }

	const static Color Black, White, Red, Green, Blue, Magenta, Cyan, Yellow, Gray, Orange, Purple;  // use Color::Black
};
 
enum class Key {
	Unknown, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	LEFT, RIGHT, UP, DOWN, SPACE, ENTER, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };




namespace G2D
{
	// key management 
	bool IsKeyPressed(Key k); 

	// Main Graphic System Function

	void InitWindow(int & argc, char** & argv, V2 WindowSize, V2 WindowStartPos, std::string name);
	void Run( void logic(), void render() );
	void ClearScreen(Color c);
	void Show();  // send all draw commands 
 
	// Mouse event
	void GetMouseButtonInfo(int idButton, bool & clicked, int & x, int & y);

	// Font
	void DrawStringFontMono (V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);
	void DrawStringFontRoman(V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);

	// Crée une texture à partir d'une description 1 lettre = 1 couleur / voir exemple
	int InitTextureFromString(V2 & Size, const std::string & Sprite);

	// Affiche un rectangle texturé à la position (x,y) (x+w,y+h)
	// angleDef produit une rotation du rectangle autours de son centre
	int  ExtractTextureFromPPM(const std::string & filepath, bool applyTransparency);
	void DrawRectWithTexture(int IDtexture, V2 pos, V2 size, float angleDeg = 0);
	void DrawRectangle(V2 P1, V2 Size, Color c, bool fill = false);
	void DrawCircle(V2 C, float r, Color c, bool fill = false);
 
	// Draw Geometry
	void DrawLine(V2 P1, V2 P2, Color c );
	void DrawPolygon(std::vector<V2> & PointList, Color c, bool fill = false);

	// Timing function
	double ElapsedTimeFromLastCallbackSeconds();
	double ElapsedTimeFromStartSeconds();


}