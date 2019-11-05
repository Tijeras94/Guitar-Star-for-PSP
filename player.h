/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <deque>
#include "oggplayer.h"
#include "midiloader.h"
#include "main.h"
#include "packets.h"
#include "config.h"
#include "data.h"
#include "pad.h"

/**
 * les différents etat du jeu
 */
enum PLAYER_STATES{
	play,
	inPause,
	inDebug,
	score,
	finished,
#ifdef AD_HOC
	inPauseMulti
#endif
};

/**
 * classe représentant la potition des étoiles a l'écran
 */
class Etoile{
public:
	int x,y;
	clock_t timeStart;
};

class Particule{
public:
	float angle;
	int length;
	clock_t timeStart;
};

/**
 * classe qui s'occupe de la partie "en jeu"
 */
class Player{
private:
//etat actuel de jeu
PLAYER_STATES state;

//variables pour le score
int mult;//coeff multiplicateur des points
int points;// le score de joueur
int showedPoints;// le score de joueur affiché a l'écran
int expectedPoints;// les prochains points qui vont etre gagné
int suite;//nombre de notes jouée de suite actuellement
int accuracy;//utilisé pour le score final, précision en pourcent
int longestStreak;//utilisé pour le score final, la plus longe suite

//utilisé pour le menu de pause
int motion; //utilisé pour l'equation différentielle responsable du mouvement de texte
int pauseSelection; //entrée de munu séléctionné en pause

//variables temporelles
float oggTime;//position actuel dans la musique en ms
clock_t timeNow, timeLast;// timeNow-timeLast = temps en micro secondes de la boucle de rendu

//listes des notes
std::vector<Note*> notes[5];

float velocity;//TODO supprimer si non utilisé
std::deque<Etoile*> etoileList;
std::deque<Particule*> particulesList[5];
bool songPresent;
bool rhythmPresent;

SceKernelUtilsMt19937Context ctx;

//les chemins vers les fichiers
char songPath[100];
char guitarPath[100];
char rhythmPath[100];
char notesPath[100];
char temp[100];

bool touches[5];//pour savoir si les touches sont pressée a la dèrnière boucle
bool firetouches[5]; //pour savoir si les touches sont pressée dans la boucle boucle et pas avant

#ifdef DEBUG
int SelectedOption;
int decalage;

/**
 * affiche le menu de debugage a l'écran
 */
void ShowDebug();
#endif

#ifdef AD_HOC
bool adhocActived;
float lastSendPing;

clock_t ping;
clock_t startMusicTime;
float NetRequestDelay;

/*int pointsMulti;
int accuracyMulti;
int longestStreakMulti;*/
std::vector<ScorePacket*> scoreList;
#endif

/**
 * affiche le jeu a l'écran
 */
void ShowScore();

/**
 * doit etre appellé a chaque note ratée
 */
void erreur();

/**
 * doit etre appelé a chaque note bien jouée
 * 
 * @param noteTime la longeur de la note en ms
 */
void noerreur(int noteTime);

/**
 * assigne les valeures au tableaux touches (qui indique les notes pressée) et firetouches (qui indique les notes venant juste d'etre pressée) a partir de la config
 */
void touchesValSet();

/**
 * assigne les valeures au tableaux touches (qui indique les notes pressée) et firetouches (qui indique les notes venant juste d'etre pressée)
 * 
 * @param touchenb le numero de la note (de 0 à 4)
 * @param pressed vrai ssi la touche est préssée
 */
void touchesValSet(int touchenb, bool pressed);

/**
 * passe le jeu a l'état 'jeu fini'
 */
void stop();

/**
 * retourne la couleur de la note i (de 0 a 4) avec la transparence alpha
 * 
 * @param i la note
 * @param alpha la transparence
 */
unsigned int getColor(int i, float alpha);

/**
 * retourne la couleur de la note i (de 0 a 4) avec la transparence alpha en surbriance
 * 
 * @param i la note
 * @param alpha la transparence
 */
unsigned int getHightColor(int i, float alpha);

/**
 * rend la scène a l'écran
 */
void renderScreen();

public:

/**
 * crée une instance de la classe
 * 
 * @param adhoc vrai ssi ce player joue sur une partie en hadhoc
 */
Player(bool adhoc);

/**
 * destructeur
 */
~Player();

/**
 * charge une musique, doit etre appellé une seul fois et avant d'appeler reset
 * 
 * @param path le chemin où se situe les différents fichiers composant la musique
 * @param difficulty la difficulté a charger (de 0 pour super easy a 3 pour expert)
 */
void loadMusic(char* path, int difficulty);

/**
 * lance (ou relance) la musique
 */
void reset();

/**
 * retourne vrai ssi le jeu est en cours
 * 
 * @return vrai ssi le jeu est en cours
 */
bool running();

#ifdef AD_HOC
/**
 * active les communication adhoc
 */
//void adhocActivate();

/**
 * appellé a la récéption d'un packet pong par pour le transmettre au player
 * 
 * @param t packet de pong recu par adhoc
 */
void pongPacket(PingPacket *t);

/**
 * appellé a la récéption d'un packet de score pour le transmettre au player
 * 
 * @param p packet de score recu par adhoc
 */
void scorePacket(ScorePacket *p);
#endif

/**
 * rend le jeu a l'écran doit etre appéllé a chaque boucle du rendu
 */
void render();
};

#endif
