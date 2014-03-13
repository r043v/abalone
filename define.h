
#ifndef _define_
#define _define_

#include <stdio.h>
//#include <stdlib.h>
#include <string.h> // pour avoir memset

// decommenter pour activer le mode 'debug', les info de debug serons sauvé dans 'abalone.log'
#define DEBUG

// point de regle, un pion seul peu t'il deplacer un autre pion ?
#define unPionPeuEnDeplacerUnAutre

// autre point de regle, si un groupe ne se deplace pas sur son vecteur,
// les pions serons deplacé independament,
// mais si un seul est bloqué, bloque t'il le deplacement des autres ?
#define unPionBloqueBloqueLesAutre


/* define pour gerer plus simplement les directions ...

    1
 32   2
 16   4
    8
*/

#define UP			1
#define UPRIGHT		2
#define DOWNRIGHT	4
#define DOWN		8
#define DOWNLEFT	16
#define UPLEFT		32


// define pour clarifier le code..
#define u32 unsigned int


// structure position, defini une coordonnée
struct position
{	u32 colon, line;
};


// structure group, défini un groupe de pion
struct group
{	struct position start_pos, end_pos; // position de debut et fin..
	struct position mid_pos; // position intermediaire (utilisé pour une taille de 3)
	u32 way, size, value; // direction, taille et code joueur
};


// macro pour simplifier la gestion des structures position
// apres avoir fini ce jeux, je me dis qu'une classe aurais pu etre mieux.

// copyPos copie la position p1 dans la position p2
#define copyPos(p1,p2) {p2->colon=p1->colon;p2->line=p1->line;}

// flipPos se charge d'intervertir le contenu des deux position données
#define flipPos(p1,p2)	   {struct position tmp;\
							tmp.colon=p1->colon;tmp.line=p1->line;\
							p1->colon=p2->colon;p1->line=p2->line;\
							p2->colon=tmp.colon;p2->line=tmp.line;}

// macro pour afficher le contenu d'une structure 'group' dans le log
#define debugGroup(grp)	abalog->printl("** group of %i case, way %i ",grp->size,grp->way);\
						abalog->printl("[%i.%i] ",grp->start_pos.colon,grp->start_pos.line);\
						if(grp->size == 3) abalog->printl("[%i.%i] ",grp->mid_pos.colon,grp->mid_pos.line);\
						abalog->printlr("[%i.%i]",grp->end_pos.colon,grp->end_pos.line)


// define pour le scan 'recursif'
#define END_OF_ARRAY 1
#define NOTHING 0
#define OUR_PION 2


// define pour le getAdjacentCase
#define AGAUCHE  0
#define AUCENTRE 1
#define ADROITE  2

// definition de couleur pour la console
#define ROUGE (FOREGROUND_RED|FOREGROUND_INTENSITY)
#define VERT  (FOREGROUND_GREEN|FOREGROUND_INTENSITY)
#define BLEU  (FOREGROUND_BLUE|FOREGROUND_INTENSITY)
#define JAUNE (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY)
#define BLANC FOREGROUND_INTENSITY

// couleur des pions
#define PLAYER_1_COLOR ROUGE
#define PLAYER_2_COLOR VERT

// couleur des info pour selectionner une colone
#define INFO_COLOR_1 JAUNE
#define INFO_COLOR_2 VERT

// constante pour effacer les info affichées.. utilisé avec plateau::deleteSelection
#define LINEINFO 2
#define GROUPINFO 4
#define SELECTINFO 8

#endif
