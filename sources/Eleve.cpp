 
#pragma warning( disable : 4996 ) 
 
#include <cstdlib>
#include <thread>
#include <fstream>
#include "G2D.h"
#include "Elements.h"
 
#define BRICK_POINTS 100
#define DELAY 0.2
#define DELAI_TOUCHE 0.14
#define BALL_COLOR Color::Red

using namespace std;
  
const double PI = 3.14159265358;

//declarations
V2 rebond(V2 vitesse, V2 normal);
V2 collisionBords(V2 newPos, V2 speed, float rayon);

void menu();
void options();
void chargement();
void jeu();
void gameOver();
void bestPlayer();

bool afficher_cotes_actifs = false;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				GAMEDATA
//
////////////////////////////////////////////////////////////////////////////////////////////////////

string filename = "score.txt";
fstream file_out;
const V2 init = V2(300, BALLRAD + 10);

struct GameData
{
	int difficulty = 0;
	int score;
	int bestScore;
	string bestPlayer;
	int turn;

	int heighBrick = HAUTEUR_BRIQUE;
	int widthBrick = LARGEUR_BRIQUE;

	int idFrame = 0;
	int heighPix = HAUTEUR_ECRAN;
	int widthPix = LARGEUR_ECRAN;
	Rect bordsHB = Rect(V2(), V2(widthPix, heighPix - heighBrick));

	int ballRad = BALLRAD;
	V2 ballInit = init;
	V2 nextInit = init;
	V2 initSpeed;
	V2 fleche;
	double angle;
	float power;
	bool launch;
	float speed;
	int combo;

	Billes billes;

	vector<Brique> briques;

	vector<Bonus> bonus;

	vector<bool> ecrans = { true, false, false , false, false, false };

	Clavier clavier;

	vector<Color> brickLvl = { Color::Yellow, Color::Orange, Color::Red,
							   Color::Cyan, Color::Blue, Color::Purple };

	Brique* getBrick(V2 _pos);

	void addBrick();

	void addBonus();

	void initialiser();

	GameData() 
	{
		initialiser();
	}
};

GameData G;


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				CLASSES
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void Brique::init_cotes()
{
	//cotes de la brique
	vector<Segment> cotes = Rect(pos, pos + size).cotes;

	//i = indice de chaque cote
	for (int i = 0; i < 4; i++) {
		//le cote est actif par defaut
		coteActif[i] = true;
		
		//on parcourt les briques a la recherche d'un cote en commun
		for (Brique brique : G.briques) {
			//si le cote est inactif on passe au suivant
			if (!coteActif[i]) break;

			//si les brique sont confondues ont passe a la suivante
			if (brique.pos == pos) continue;

			//cotes d'une brique
			vector<Segment> briqueCotes = Rect(brique.pos, brique.pos + brique.size).cotes;

			//j = indice de chaque cote d'une autre brique
			for (int j = 0; j < 4; j++) {
				//si on trouve un cote en commun on desactive ce cote
				if (cotes[i] == briqueCotes[j]) {
					coteActif[i] = false;
					//dernier cote
					if (i == 3) return;
					//on passe au cote suivant
					i++;
					break;
				}
			}
		}
	}

	//comparaison avec les bords de l'ecran
	if (pos.x <= 0) {
		coteActif[0] = false;
	}
	if ((pos + size).x >= G.widthPix) {
		coteActif[2] = false;
	}
}
*/

/*
* La bille change de trajectoire a chaque collision avec le cote le plus proche
*/
void Bille::gestionCollisions(V2& newPos, V2& newSpd, double dt, float _distance)
{
	if (G.briques.empty()) {
		return;
	}

	//vecteur normal a un cote
	V2 normal;
	//point d'intersection entre un cote et la trajectoire
	V2 intersection;
	//indice de la brique rencontree en premier
	auto itFirstBrique = G.briques.begin();
	//distance entre la position et le cote le plus proche
	float distance = _distance;
	//collision avec un cote actif d'une brique
	bool collision_brique = false;

	//variables temporelles
	V2 normal_temp, intersection_temp;
	float distance_temp;

	//rebond sur un bord
	normal = collisionBords(newPos, newSpd, rayon);
	newSpd = rebond(newSpd, normal);
	
	//recherche de la brique la plus proche
	auto itBrick = G.briques.begin();
	while (itBrick != G.briques.end()) 
	{
		//verifie la collision et compare la distance avec distance
		if (itBrick->collisionBrique(pos, newPos, rayon, distance, &normal_temp, 
			&intersection_temp) && !itBrick->interBrick(pos))
		{
			//distance entre la position de la bille et le point de collision
			distance_temp = (intersection_temp - pos).norm();
			//actualisation des variables
			if (distance_temp < distance) {
				distance = distance_temp;
				itFirstBrique = itBrick;
				normal = normal_temp;
				intersection = intersection_temp;
				collision_brique = true;
			}	
		}
		itBrick++;
	}
	
	if (collision_brique) {
		//nouvelle vitesse
		newSpd = rebond(newSpd, normal);
		//direction de la nouvelle vitesse
		V2 direction = newSpd;
		direction.normalize();
		//modification de la position
		pos = intersection;
		newPos = pos + distance_temp * direction;

		//destruction de la brique
		if (--(itFirstBrique->resistance) <= 0)
		{
			//ajout de points
			G.score += itFirstBrique->points * G.combo * BRICK_POINTS;
			//effacement de la brique dans la liste
			G.briques.erase(itFirstBrique);
			G.combo++;
		}

		gestionCollisions(newPos, newSpd, dt, distance);
	}
}


void Bille::gestionRebond(double dt)
{
		V2 newPos = pos + speed * dt;
		V2 newSpd = speed;
		float distance = speed.norm() * dt;
		
		//desactivation des bonus attrapes
		if (!G.bonus.empty()) {
			for(Bonus& bonus : G.bonus) {
				bonus.bonusCaught(pos, rayon);
			}
		}

		//permet de reinitialiser les cotes seulement si necessaire
		//bool reinit = false;

		//rebond 
		if (speed != 0) {
			gestionCollisions(newPos, newSpd, dt, distance);
		}

		speed = newSpd;
		pos = newPos;
}


void Billes::envoi(double dt, int& idx)
{
	const double delay = (4 * G.ballRad + 10) / G.initSpeed.norm();

	if (!empty()) {
		if (timer_envoi > delay) {
			if (idx < nbBilles - 1) {
				push_back(Bille(G.ballInit, G.initSpeed, G.ballRad));
				//cout << "bille : " << idx + 2 << "/" << nbBilles << endl;
				timer_envoi = 0;
				idx++;
			}
		}
		else
			timer_envoi += dt;
	}
}


Brique* GameData::getBrick(V2 _pos)
{
	for (Brique& brique : briques) {
		if (brique.interBrick(_pos))
			return &brique;
	}
	return NULL;
}


void GameData::addBrick()
{
	int res = turn;
	int rd = rand() % (widthPix / widthBrick);
	V2 pos = V2(rd * widthBrick, bordsHB.C.y - 2 * heighBrick);

	int k = briques.size() - 1;

	while ((k >= 0) && (k >= briques.size() - 7)) {
		Brique& brique = briques[k];
		if (brique.interBrick(pos + 5)) {
			if (brique.resistance <= res)
				brique.resistance *= 2;
			brique.points = brique.resistance;
			return;
		}
		k--;
	}

	briques.push_back(Brique(pos, res));
}


void GameData::addBonus()
{
	static int countdown = 5;
	static V2 pos;

	while (countdown > 0) {
		int rd = rand() % (widthPix / widthBrick);
		pos = V2(rd * widthBrick + LARGEUR_BONUS,
			bordsHB.C.y - 2 * heighBrick + HAUTEUR_BONUS);

		bool inter = false;
		for (Brique& brique : briques) {
			if (brique.interBrick(pos)) {
				countdown--;
				inter = true;
			}
		}
		if (!inter) {
			bonus.push_back(Bonus(pos));
			break;
		}
	}

	countdown = 5;
}


void GameData::initialiser()
{
	score = 0;
	turn = 1;

	fleche = V2(0, 1);
	angle = PI / 2;
	power = 1;
	launch = true;
	speed = 400;
	combo = 1;

	briques.clear();
	billes.initialiser();
	bonus.clear();
	billes.nbBilles = 1;
	clavier.currentTouch = 0;

	addBrick();
	addBonus();
	billes.push_back(ballInit, V2(), ballRad);
}


/////////////////////////////////////////////////////////////////////////////////////////
// 
//		LOGIQUE
// 
/////////////////////////////////////////////////////////////////////////////////////////


void gestionBriques()
{
	for (Brique& brique : G.briques) {
		brique.descend();
	}
	for (int i = 0; i < sqrt(G.turn + G.difficulty); i++) {
		G.addBrick();
	}
	/*
	for (Brique& brique : G.briques) {
		brique.init_cotes();
	}
	*/
	if (G.briques[0].pos.y < HAUTEUR_LANCEMENT /*+ HAUTEUR_BRIQUE*/ ) {
		gameOver();
	}
}


void gestionBonus()
{
	if (!G.bonus.empty()) {
		auto itBonus = G.bonus.begin();
		while (itBonus != G.bonus.end()) {
			itBonus->descend();
			if (itBonus->caught) {
				G.billes.nbBilles++;
			}
			else if (!(itBonus->pos.y < HAUTEUR_LANCEMENT + HAUTEUR_BRIQUE)) {
				itBonus++;
				continue;
			}
			itBonus = G.bonus.erase(itBonus);
		}
	}

	G.addBonus();
}


void gestionEnvoi(double& dt)
{
	const double angle_elementaire = PI / 50;
	static double delai = 0;

	if (delai > 0.03) {
		if (G2D::IsKeyPressed(Key::SPACE)) {
			G.launch = false;
			G.initSpeed = G.billes.back().speed = G.fleche * G.speed;
			return;
		}

		if (G2D::IsKeyPressed(Key::LEFT) && (G.angle < PI - 6 * angle_elementaire)) {
			G.angle += angle_elementaire;
		}
		else if (G2D::IsKeyPressed(Key::RIGHT) && (G.angle > 6 * angle_elementaire)) {
			G.angle -= angle_elementaire;
		}
		else if (G2D::IsKeyPressed(Key::UP) && (G.power < 2)) {
			G.power += 0.1;
		}
		else if (G2D::IsKeyPressed(Key::DOWN) && (G.power > 0.5)) {
			G.power -= 0.1;
		}
		else {
			return;
		}
		G.fleche = G.power * V2(G.angle);
		delai = 0;
	}
	else {
		delai += dt;
	}
}


//
void gestionEnvoye(double& dt, bool& init)
{
	static int idx_bille;

	if (G.billes.empty() || ((G2D::IsKeyPressed(Key::ENTER)))) {
		if (!G.billes.empty())
			G.billes.clear();
		G.turn++;

		gestionBriques();
		gestionBonus();

		init = true;
		G.ballInit = G.nextInit;
		G.billes.push_back(G.ballInit, V2(), G.ballRad);
		idx_bille = 0;

		G.fleche = V2(0, G.power);
		G.angle = PI / 2;
		G.launch = true;
		//G.power = 1;
		G.combo = 1;

		G.billes.timer_envoi = 0;
	}
	else {
		auto it = G.billes.begin();
		while (it != G.billes.end()) {
			if (it->pos.y < 0 - it->rayon) {
				//change la position de lancer en la position de la premiere bille a etre revenue
				if (init) {
					G.nextInit = V2(it->pos.x, G.ballInit.y);
					init = false; //la position de lancer a ete modifiee
				}
				//supprime la bille du vecteur billes
				it = G.billes.erase(it);
			}
			else {
				++it;
			}
		}
		G.billes.envoi(dt, idx_bille);
	}
}


//retourne la vitesse modifiee par un rebond
V2 rebond(V2 v, V2 n)
{
	if (n == V2()) return v;

	n.normalize();
	V2 t = V2(n.y, -n.x);
	float vt = v * t;
	float vn = v * n;
	V2 r = vt * t - vn * n;

	return r;
}

/*
//retourne un vecteur normal a un bord ou nul
bool collisionBords(V2 pos, V2 newPos, float rayon, float distance, 
	V2* normal = nullptr, V2* intersection = nullptr)
{
	Segment trajectoire = Segment(pos, newPos);
	vector<Segment> bords;

	for (int i = 0; i < 3; i++) {
		bords.push_back(G.bordsHB.cotes[i]);
	}

	return collisionSegment(trajectoire, rayon, distance, bords, nullptr, normal, intersection);
}
*/


//retourne un vecteur normal a un bord ou nul
V2 collisionBords(V2 newPos, V2 speed, float rayon)
{
	if ((newPos.x - rayon < 0) && (rebond(speed, V2(1, 0)).x > 0)) {
		return V2(1, 0);
	}
	else if ((newPos.y + rayon > G.bordsHB.C.y) && (rebond(speed, V2(0, -1)).y < 0)) {
		return V2(0, -1);
	}
	else if ((newPos.x + rayon > G.widthPix) && (rebond(speed, V2(-1, 0)).x < 0)) {
		return V2(-1, 0);
	}
	return V2();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				FENETRES
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//active un ecran et desactive les autres
void activerEcran(int idx)
{
	for (unsigned int i = 0; i < G.ecrans.size(); i++) {
		if (i == idx)
			G.ecrans[i] = true;
		else
			G.ecrans[i] = false;
	}
}

//changements d'ecran
void menu() { activerEcran(0); }

void options() { activerEcran(1); }

void chargement() { activerEcran(2);	G.initialiser(); }

void jeu() { activerEcran(3); }

void gameOver() { activerEcran(4); }

void bestPlayer()
{
	//si meilleur score passage a l'ecran best player
	if (G.score > G.bestScore) {
		G.bestPlayer = "";
		activerEcran(5);
	}
	//sinon retour au menu
	else {
		activerEcran(0);
	}
}


void gestionMenu(double& timer, double dt)
{
	//ecran du menu principal
	if (!G.ecrans[0]) {
		return;
	}
	//selection des options
	if (G2D::IsKeyPressed(Key::ENTER) && (timer > DELAY)) {
		timer = 0;
		options();
	}
	timer += dt;
}


void gestionOptions(double& timer, double& delay, double& dt)
{
	static bool pushed = false;

	//ecran du menu options
	if (!G.ecrans[1]) {
		return;
	}

	//delai d'activation
	if (timer < DELAY) {
		timer += dt;
		return;
	}

	//lancement du jeu
	if (G2D::IsKeyPressed(Key::ENTER)) {
		timer = 0;
		chargement();
	}

	//delai d'activation d'une touche
	if (delay < 0.2) {
		delay += dt;
		return;
	}
	//selection de la difficulte (0 a 5)
	if (G2D::IsKeyPressed(Key::LEFT) && (G.difficulty > 0)) {
		G.difficulty--;
		delay = 0;
	}
	if (G2D::IsKeyPressed(Key::RIGHT) && (G.difficulty < 5)) {
		G.difficulty++;
		delay = 0;
	}
}


void gestionChargement(double& timer, double dt)
{
	//ecran de chargement
	if (!G.ecrans[2]) {
		return;
	}
	//delai d'activation
	if (timer > DELAY) {
		timer = 0;
		jeu();
	}
	timer += dt;
}


void gestionJeu(double& timer, double dt)
{
	//vaut true tant qu'une premiere bille n'est pas revenue
	static bool init = true;

	//ecran de jeu
	if (!G.ecrans[3]) {
		return;
	}

	//delai d'activation
	if (timer < DELAY) {
		timer += dt;
		return;
	}

	//interrompre la partie
	if (G2D::IsKeyPressed(Key::M)) {
		timer = 0;
		bestPlayer();
	}
	//afficher l'ecran best player
	else if (G2D::IsKeyPressed(Key::B)) {
		timer = 0;
		G.bestPlayer = "";
		activerEcran(5);
	}
	//afficher les segments actifs
	else if (G2D::IsKeyPressed(Key::A)) {
		timer = 0;
		afficher_cotes_actifs = !afficher_cotes_actifs;
	}

	//phase de visee
	if (G.launch) {
		gestionEnvoi(dt);
	}
	//phase d'envoi
	else {
		gestionEnvoye(dt, init);
	}

	//rebond des billes
	if (!G.billes.empty()) {
		for (Bille& bille : G.billes) {
			bille.gestionRebond(dt);
		}
	}
}


void gestionGameOver(double& timer, double dt)
{
	//ecran game over
	if (!G.ecrans[4]) {
		return;
	}

	//delai d'activation
	if (timer < DELAY) {
		timer += dt;
		return;
	}
	//changement d'ecran
	if (G2D::IsKeyPressed(Key::ENTER)) {
		timer = 0;
		bestPlayer();
	}
}


void gestionBestPlayer(double& timer, double& timer_touche, double dt)
{
	//ecran best player
	if (!G.ecrans[5]) {
		return;
	}

	//delai d'activation
	if (timer < DELAY) {
		timer += dt;
		return;
	}

	//delai d'appui sur une touche
	if (timer_touche < DELAI_TOUCHE) {
		timer_touche += dt;
		return;
	}

	//modification du meilleur score
	G.bestScore = G.score;

	int n = G.clavier.touches.size(); //nombre de touches
	int nc = G.clavier.colonnes; //nombre de colonnes
	int nl = n / nc + 1; //nombre de lignes

	int& idT = G.clavier.currentTouch; //indice de la touche
	int li = idT / nc; //ligne de idT
	int ci = idT % nc; //colonne de idT
		
	//validation d'une touche
	if (G2D::IsKeyPressed(Key::ENTER)) {
		//touche end
		if (idT == G.clavier.touches.size() - 1) {
			//ouverture du fichier score.txt
			file_out.open(filename, ios_base::out);
			if (!file_out.is_open()) {
				cout << "failed to open " << filename << endl;
				return;
			}
			//enregistrement du nom et du score
			else {
				file_out << G.bestScore << endl;
				file_out << G.bestPlayer << endl;
			}
			//retour au menu
			timer = 0;
			menu();
		}
		//touche supp
		else if (G.clavier.currentTouch == n - 2) {
			if (!G.bestPlayer.empty()) {
				G.bestPlayer.erase(G.bestPlayer.size() - 1);
			}
		}
		//ajout de caracteres
		else {
			//taille du nom <= 10
			if (G.bestPlayer.size() < 10) {
				G.bestPlayer += G.clavier.touches[idT].texture;
			}
		}
	}
	//deplacement horizontal
	else if (G2D::IsKeyPressed(Key::LEFT)) {
		idT = li * nc + (idT + nc - 1) % nc;
		if (idT >= n) idT = n - 1;
	}
	else if (G2D::IsKeyPressed(Key::RIGHT)) {
		idT = li * nc + (idT + 1) % nc;
		if (idT >= n) idT = n - 1;
	}
	//deplacement vertical
	else if (G2D::IsKeyPressed(Key::UP)) {
		idT = ((li + nl - 1) % nl) * nc + ci;
		if (idT >= n) idT -= nc;
	}
	else if (G2D::IsKeyPressed(Key::DOWN)) {
		idT += nc;
		if (idT >= n) idT = ci;
	}
	//aucune touche n'est appuyee
	else {
		return;
	}
	//appui sur une touche
	timer_touche = 0;	
}


void logic()
{
	G.idFrame += 1;
	double dt = G2D::ElapsedTimeFromLastCallbackSeconds();
	static double timer_menu = 0; //used to disable the enter key
	static double timer_touche = 0; //used to disable the keys
	static double timer_delay = 1; //used in options to chose difficulty

	gestionMenu(timer_menu, dt);
	gestionOptions(timer_menu, timer_delay, dt);
	gestionChargement(timer_menu, dt);
	gestionJeu(timer_menu, dt);
	gestionGameOver(timer_menu, dt);
	gestionBestPlayer(timer_menu, timer_touche, dt);
}


/////////////////////////////////////////////////////////////////////////////////////////
// 
//		AFFICHAGE
// 
/////////////////////////////////////////////////////////////////////////////////////////

void centrerStringFontRoman(V2 pos, V2 caseSize, string str, float fontSize = 20.0, float thickness = 3.0,
	Color col = Color::White)
{
	Touches touches;
	int len = str.length();

	for (int i = 0; i < len; i++) {
		string text = str.substr(i, 1);
		Element elt = touches.getTouch(text);

		elt.pos = pos + V2((i * caseSize.x) - (caseSize.x * len / 2), -caseSize.y / 2);

		G2D::DrawStringFontRoman(elt.pos, text, fontSize, thickness, col);
	}
}

void centrerStringFontRoman(V2 pos, string str, float fontSize = 20.0, float thickness = 3.0,
	Color col = Color::White)
{
	float size = 4 * fontSize / 5;
	V2 caseSize = V2(size, size);

	centrerStringFontRoman(pos, caseSize, str, fontSize, thickness, col);
}


void centrerStringFontMono(V2 pos, V2 caseSize, string str, float fontSize = 20.0, float thickness = 3.0,
	Color col = Color::White)
{
	Touches touches;
	int len = str.length();

	for (int i = 0; i < len; i++) {
		string text = str.substr(i, 1);
		Element elt = touches.getTouch(text);

		elt.pos = pos + V2((i * caseSize.x) - (caseSize.x * len / 2), -caseSize.y / 2);

		G2D::DrawStringFontMono(elt.pos, text, fontSize, thickness, col);
	}
}

void centrerStringFontMono(V2 pos, string str, float fontSize = 20.0, float thickness = 3.0,
	Color col = Color::White)
{
	float size = 4 * fontSize / 5;
	V2 caseSize = V2(size, size);

	centrerStringFontMono(pos, caseSize, str, fontSize, thickness, col);
}


void assetsInit()
{

}


void affichageBriques()
{
	for (Brique brique : G.briques) {
		int i = 0;
		for (int k = 1; k <= 6; k++) {
			if (3 * brique.resistance > k * G.turn) {
				i++;
			}
		}

		G2D::DrawRectangle(brique.pos, brique.size, G.brickLvl[i], true);
		centrerStringFontMono(brique.pos + brique.size / 2, 
			to_string(brique.resistance), 15.0, 1.0, Color::Black);

		if (afficher_cotes_actifs) {
			Rect briqueHB = Rect(brique.pos, brique.pos + brique.size);
			for (int i = 0; i < 4; i++) {
				if (brique.coteActif[i]) {
					for (int j = 0; j < 4; j++) G2D::DrawLine(briqueHB.cotes[i].A, briqueHB.cotes[i].B, Color::White);
				}
			}
		}
	}
}


void affichageBonus()
{
	for (Bonus bonus : G.bonus) {
		if (!bonus.caught) {
			//bonus.idTex = G2D::InitTextureFromString(bonus.size, bonus.texture);
			//G2D::DrawRectWithTexture(bonus.idTex, bonus.pos, bonus.size);

			G2D::DrawRectangle(bonus.pos, bonus.size, Color::Green, true);
		}
	}
}


string getLine(int idx)
{
	ifstream input_file(filename);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '" << filename << "'" << endl;
		return "";
	}

	string line;
	for (int i = 0; i <= idx; i++) {
		getline(input_file, line);
	}
	input_file.close();

	return line;
}


void afficherMenu()
{
	if (G.ecrans[0]) {
		
		centrerStringFontRoman(V2(G.widthPix / 2, G.heighPix / 2),
			"BRICK BREAKER", 50, 5.0, Color::Yellow);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 60),
			"Press ENTER to start", 20.0, 2.0, Color::White);
		
		G.bestScore = stoi(getLine(0));
		G.bestPlayer = getLine(1);
		

		if (G.bestScore != 0) {
			centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 130),
				"Leaderboard", 25, 2, Color::Red);
			centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 160), 
				G.bestPlayer + " " + to_string(G.bestScore), 20, 2, Color::Red);
		}
	}
}


void afficherOptions()
{
	if (G.ecrans[1]) {
		string diff_str = "Difficulty : " + to_string(G.difficulty);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2),
			"OPTIONS", 50, 5, Color::Green);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 60),
			diff_str, 20, 2, Color::White);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 90),
			"Press ENTER to start", 20, 2, Color::White);
	}
}


void afficherChargement()
{
	if (G.ecrans[2]) {}
}


void afficherJeu()
{
	if (G.ecrans[3]) {
		string score = "Score : " + to_string(G.score);

		//affiche le plateau
		G2D::DrawStringFontMono(V2(15, G.heighPix - 20), string(score), 20, 2, Color::White);
		G2D::DrawStringFontMono(V2(G.widthPix - 200, G.heighPix - 20), 
			"Menu   : M", 20, 2, Color::White);
		G2D::DrawStringFontMono(V2(G.widthPix - 200, G.heighPix - 40),
			"Launch : SPACE", 20, 2, Color::White);
		G2D::DrawStringFontMono(V2(G.widthPix - 200, G.heighPix - 60),
			"Stop   : ENTER", 20, 2, Color::White);
		G2D::DrawRectangle(V2(0, 0), V2(G.widthPix, G.heighPix), Color::Green);
		G2D::DrawLine(V2(0, HAUTEUR_LANCEMENT), V2(G.widthPix, HAUTEUR_LANCEMENT), Color::White);
		G2D::DrawLine(V2(0, G.bordsHB.C.y), V2(G.widthPix, G.bordsHB.C.y), Color::White);

		affichageBriques();
		affichageBonus();

		//affiche la fleche directionnelle
		if (G.launch) {
			for (int i = 0; i < 3; i++) {
				G2D::DrawLine(G.ballInit, G.ballInit + 50 * G.fleche, Color::Yellow);
			}
		}

		//affiche les billes
		for (Bille bille : G.billes) {
			G2D::DrawCircle(bille.pos, bille.rayon, BALL_COLOR, true);
		}
	}
}


void afficherGameOver()
{
	if (G.ecrans[4]) {
		centrerStringFontRoman(V2(G.widthPix / 2, G.heighPix / 2), 
			"GAME OVER", 50, 5, Color::Red);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix / 2 - 60), 
			"Press ENTER to leave", 20, 2, Color::White);
	}
}


void afficherBestPlayer()
{
	if (G.ecrans[5]) {
		centrerStringFontRoman(V2(G.widthPix / 2, G.heighPix - 50), 
			"BEST PLAYER", 50, 5, Color::Green);
		centrerStringFontMono(V2(G.widthPix / 2, G.heighPix - 90), 
			"NAME : " + G.bestPlayer, 30.0, 2.5, Color::White);
		for (Element elt : G.clavier.touches) {
			G2D::DrawStringFontMono(elt.pos, elt.texture, 40, 3, Color::White);
		}
		Element currentTouch = G.clavier.touches[G.clavier.currentTouch];
		G2D::DrawStringFontMono(currentTouch.pos, currentTouch.texture, 40, 3, Color::Red);
	}
}


void render()
{
	G2D::ClearScreen(Color::Black);

	afficherMenu();
	afficherOptions();
	afficherChargement();
	afficherJeu();
	afficherGameOver();
	afficherBestPlayer();

	G2D::Show();
}


/////////////////////////////////////////////////////////////////////////////////////////
// 
//		MAIN
// 
/////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	G2D::InitWindow(argc, argv, V2(G.widthPix, G.heighPix), V2(200, 200), string("Brick Breaker"));
	  
	assetsInit();

	G2D::Run(logic,render);
}
