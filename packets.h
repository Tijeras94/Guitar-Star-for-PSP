/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef PACKETS_H
#define PACKETS_H
// ------------------------------------------ packets de guitar star ------------------------------------------ 

//packet de base qui contient le type qui permet de l'identifier
typedef struct Packet{
	int type;
};

//envoyé par le client au serveur pour que je serveur choisise une musique
#define MUSIC_LIST 1
typedef struct MusicListPacket{
	int type;
	char name[128];
	char artist[128];
};

//envoyé par le client pour indiquer que le transfère de la liste des musiques est terminé
#define END_MUSIC_LIST 7
typedef struct EndMusicListPacket{
	int type;
};

//envoyé par le serveur pour indiquer la mesique séléctionné
#define MUSIC_SELECTED 2
typedef struct MusicSelectedPacket{
	int type;
	char name[128];
	char artist[128];
	//char dir[128];
	//bool downloadRequest;
};

//envoyé par le client pour confirmer la séléction de la musique
#define MUSIC_SELECTED_RESPONSE 3
typedef struct MusicSelectedResponsePacket{
	int type;
	bool downloadRequest;
	bool d[4]; //vrai ssi les difficultées correspondants existent (données non présentent si downloadRequest == false)
};

//envoyé pour tranferer un fichier
#define SEND_FILE 4
#define SEND_FILE_BUFFER_SIZE 0x200
typedef struct SendFilePacket{
	int type;
	bool start;//vrai ssi ce packet constitue le premier envoi d'une partie d'un fichier
	bool end;//vrai ssi ce packet consitue la dèrnière partie de l'envoi d'un fichier
	int length;
	char dir[100];
	char file[100];
	char buff[SEND_FILE_BUFFER_SIZE];
};

//envoyé lors du transfère d'un fichier par la personne qui recoi le fichier pour indiquer que le SendFilePacket suivant peut etre envoyé
#define SEND_FILE_NEXT_PACKET 10
typedef struct SendFileNextPacket{
	int type;
};

//équivalent d'un SendFilePacket dans le quel start = end = false
//ce packet est néssésairement un packet d'envoi de fichier intermédiaire de longeur SEND_FILE_BUFFER_SIZE
#define INTERMEDIATE_SEND_FILE 11
typedef struct IntermediateSendFilePacket{
	int type;
	char buff[SEND_FILE_BUFFER_SIZE];
};

//envoyé par le serveur pour indiquer au client qu'il doit charger la musique
#define LOAD_MUSIC 5
typedef struct LoadMusicPacket{
	int type;
	int difficulty;
};

//envoyé au client pour indiquer qu'il faut lancer la musique
#define START_MUSIC 6
typedef struct StartMusicPacket{
	int type;
};

/*TODO remove
#define TIME_MUSIC 8
typedef struct MusicTimePacket{
	int type;
	float time;
};*/


//packet de ping la réponse est un pong
#define PING_PACKET 12
#define PONG_PACKET 13
typedef struct PingPacket{
	int type;
	clock_t t;
};

//envoyé par un joueur pour indiquer qu'il quitte le mode multi
#define QUIT_PACKET 15
typedef struct QuitPacket{
	int type;
};

//envoyé par un joueur pour indiquer qu'il quite la musique mais pas le mode multi
#define QUIT_MUSIC_PACKET 16
typedef struct QuitMusicPacket{
	int type;
};

//envoyé a la fin d'une partie pour comparer les scores
#define SCORE_PACKET 17
typedef struct ScorePacket{
	int type;
	char name[128];
	int points;
	int accuracy;
	int longestStreak;
};

#define MUSIC_LIST_ENTER_DIR 18
typedef struct MusicListEnterDirPacket{
	int type;
	char dir[128];
};

#define MUSIC_LIST_QUIT_DIR 19
typedef struct MusicListQuitDirPacket{
	int type;
};

#define START_MUSIC_LIST 20
typedef struct StartMusicListPacket{
	int type;
};

// ------------------------------------- negociation du debut d'une partie ------------------------------------

//                        CLIENT                        |                       SERVEUR                        //
//======================================================|======================================================//
//  <state:start>                                       |  <state:start>                                       //
//                envoi de la liste des musiques        |                                                      //
//                (StartMusicListPacket)--------------------------------------->                               //
//                     (MusicList)--------------------------------------------->filtrage                       //
//                                                      |                      en attente                      //
//                    (EndMusicList)------------------------------------------->continue                       //
//                                                      |                                                      //
//======================================================|======================================================//
//  <state:musicSelection>                              |  <state:musicSelection>                              //
//                                                      |               selectionne une musique                //
//                      en attente                      |                                                      //
//                       continue<------------------------------------------(MusicSelected)                    //
//     si downloadRequest=true: envoi de la musique---------->                                                 //
//                                                      |                     en attente                       //
//               (MusicSelectedResponse)-------------------------------------->continue                        //
//                                                <-------------si downloadRequest=true: envoi de la musique   //
//                                                      |======================================================//
//                     en attente                       |  <state:difficultySelectionMulti>                    //
//                                                      |               selectionne la difficulté              //
//                 charge la musique<-----------------------------------------(LoadMusic)                      //
//                                                      |                  charge la musique                   //
//                      attente                         |                  attend 3 secondes                   //
//                     continue<---------------------------------------------(StartMusic)                      //
//======================================================|======================================================//
//  <state:playMulti>                                   |  <state:playMulti>                                   //
//                       joue                           |                       joue                           //

//une fois la partie finie la négociation reprend a l'etat 'musicSelection'
#endif
