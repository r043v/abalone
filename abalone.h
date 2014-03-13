
#ifndef _abalone_
#define _abalone_

#include "plateau.h"

// classe 'abalone', la classe primaire du jeux

class abalone
{
  public:
	u32 joueurActif;				// tout est dans le nom Oo
	plateau *game;					// le plateau de jeux
    console shell;                  // instance de la classe console, pour gerer plus facilement l'affichage

	u32 isFinish(void);				// un joueur à gagné ?
	void changerJoueurActif(void);	// interverti le joueur actuel ..
	void draw(void);				// affiche le jeux
    void getInfoFromPlayer(void);   // menu et gestion des joueurs, de la partie
	
    void initAndPlay(u32 taillePlateau);	// init de la classe
	abalone(u32 taillePlateau);		// constructeur alternatif, ou l'on specifi la taille du plateau
	abalone(void);					// constructeur standard avec une taille de plateau de 4
	~abalone(void);					// destructeur ..

  private:
    u32 askPosition(struct position *pos, const char *txt);
    u32 askWay(const char *txt);
    void drawPlayerInfoAndArray(void);
    u32 taillePlateau;
   	void play(void); // lance la partie
};

#endif
