/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

/**
 * initialise la bibliothèque
 * 
 * @param channel le numero du canal audio de la psp a utiliser
 */
void OGG_Init(int channel);

/**
 * lance la lecture de la musique
 * 
 * @return vrai ssi la musique n'était pas deja en lecture
 */
bool OGG_Play();

/**
 * met en pause la musique
 */
void OGG_Pause();

/**
 * stoppe la lecture de la musique
 */
void OGG_Stop();

/**
 * stoppe la musique et libère les ressources
 */
void OGG_End();

/**
 * charge une musique
 * 
 * @param filename le chemin vers le fichier a charger
 * @return vrai ssi le chargement est réussi
 */
bool OGG_Load(char *filename);

/**
 * libère les ressources
 */
void OGG_FreeTune();

/**
 * donne la position actuelle dans la musique en milisecondes
 * 
 * @return le position dans la musique en ms
 */
unsigned int OGG_GetTime();

/**
 * indique c'est la musique est finie
 * 
 * @return vrai ssi la musique est finie
 */
bool OGG_EndOfStream();

/**
 * augmente la vitesse de lecture de la musique
 */
void accelerate();

/**
 * retourne a la vitesse de lecture normale
 */
void stopAccelerate();
