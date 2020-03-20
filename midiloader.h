/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef MIDILOADER_H
#define MIDILOADER_H
#include <vector>
#include "jdkmidi/world.h"
#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/parser.h"
#include "jdkmidi/fileread.h"
#include "jdkmidi/track.h"
#include "jdkmidi/multitrack.h"
#include "jdkmidi/filereadmultitrack.h"
#include "jdkmidi/fileshow.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/manager.h"
#include "jdkmidi/driverdump.h"
#include <dirent.h>
#include <pspiofilemgr.h>
#include <string.h>
#include "data.h"
#include "typedef.h"
#include "packets.h"
#include <pspthreadman.h>
#include "config.h" // contains strlower() fuction
#ifdef AD_HOC
#include "matchingWifi/adhoc.h"
#endif


typedef struct Note{
	int debut,fin,note;// temps de debut et de fin de la note et la valeur de la note de 0 a 4
	bool played,missed,pressed,released;//indique si la noté a été joué et raté; si elle est pressé et relaché
	int releasetime;// indique le temps auquel la noté a été relaché
	#ifdef DEBUG
	int baseDebut, baseFin;
	#endif
};

/**
 * fonction qui prend en argument le chemin vers un fichier midi et qui retourne le coefficient multiplicateur de la vitesse du fichier midi
 * spécifications midi utilisé: http://www.sonicspot.com/guide/midifiles.html
 * 
 * @param filename le chemin vers le fichier
 * @return le coefficient multiplicateur de la vitesse de la musique
 */
float getVelocity(char* filename);

/*
 * charge le fichier midi dans la variable notes
 *
 * @param filename le chemin vers le fichier midi
 * @param notes un pointeur vers un tableau de 5 vecteur de Note qui sera chargé
 * @param difficulty le niveau de difficulté (compris entre 0 pour très facile et 3 pour expert)
 */
void loadmidifile(char* filename, std::vector<Note*>* notes,int difficulty);


/**
 * recherche la musique portant le nom 'name' dans la liste de musique 'musicList'
 * 
 * @param name le nom de la musique a chercher
 * @param musicList la liste dans laquelle chercher
 * @return l'occurence de la musique si elle est dans la liste sinon 0
 */
MusicEntry* searchMusic(char* name, FolderEntry* list);

/**
 * donne le nombre de notes dans le fichier midi 'filename' a la difficulté 'difficulty'
 * 
 * @param filename le chemin vers le fichier midi
 * @param difficulty ne niveau de difficulté
 * @return le nombre de notes
 */
int numberOfNotes(char* filename, int difficulty);

/**
 * calcul en pourcent le rapport "nb de notes jouées"/"nb de notes totale" 
 * 
 * @param notes le tableau de 5 vecteur Note dans lequel il faut calculer
 * @return la précision en pourcent
 */
int accuracyPerCent(std::vector<Note*>* notes);

/**
 * place dans 'name' et 'artist' le nom et l'auteur contenu dans le fichier song.ini 'filename'
 * 
 * @param filename le chemin vers le fichier
 * @param name un pointer dans lequel sera placé le nom de la chanson
 * @param artist un pointeur dans lequel sera placé le nom de l'artiste
 */
void iniInfo(char* filename, char* name, char* artist);


FolderEntry* loadFileList();

#ifdef AD_HOC

/**
 * donne une chaine de texte contenant un message indiquant l'avancement de transfère des fichier
 * 
 * @return le message
 */
char* getSendFileState();

/**
 * envoi un fichier
 * 
 * @param folder le chemin du dossier contenant le fichier
 * @param file le nom du fichier
 * @return 0 en cas de succes sinon un code d'erreur
 */
int clientSendFile(char* folder, char* file);

void notifySendFileNextPacket();

/**
 * envoi les fichier "guitar.ogg", "notes.mid", "song.ini" et "song.ogg" contenu dans un dossier
 * 
 * @param folder le dossier a envoyer
 */
void ClientSendMusic(char* folder);

void sendClientMusicList(FolderEntry* folder);
#endif

/**
 * se place dans l'arborésence dir en créant les dossiers si ils n'existent pas (dir de forme "truc/bidule/machin/")
 *
 * @param dir le dossier dans lequel se placer
 */
void enterDir(char* dir);

#endif
