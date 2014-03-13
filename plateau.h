
#ifndef _plateau_
#define _plateau_

#include "define.h" // header contenant diverse constante/macro, l'inclusion d'header standard et l'activation du mode debug
#include "log.h" // ajout de l'header de la classe 'log_file', pour debuguer
#include "console.h" // header pour manipuler plus simplement la sortie en mode console

// classe plateau, gere le plateau, les pions, deplacements ...

class plateau
{	public:

	plateau(u32 taille);// init..
	~plateau(void);		// et free...
	
	// fonction d'affichage
	void draw2log(void);
	void draw2cmd(void);
	void draw2wnd(void);

	// test de validit�, retourne 0 ou 1..
	u32 isPosValid(u32 colon, u32 line);
	u32 isPosValid(struct position *pos);
	
	// set et get.. retourne 0 en cas d'erreur, sinon renvoi 1
	u32 set(u32 colon, u32 line, u32 value);
	u32 get(u32 colon, u32 line);
	u32 set(struct position *pos, u32 value);
	u32 get(struct position *pos);

	// fonction indiquant si deux positions sont adjacentes,
	// retourne la direction entre la premiere et la seconde, ou 0 si non adjacent.
	u32 isAdjacent(struct position *p1, struct position *p2);

	// fonction deplacant un seul pion, selon la direction indiqu�e, ou sur la case indiqu�e,
	// retourne 0 si le deplacement est impossible, sinon, renvoi 1..
	u32 move(struct position *pos, u32 way);
	u32 move(struct position *src, struct position *dst);

	// fonction deplacant un groupe de pion, retourne 0 en cas d'impossibilit�e, sinon renvoi 1
	u32 move(struct group *grp, u32 way);
	u32 move(struct group *grp, struct position *pos);
	
	// fonction indiquant si le pion actuellement dans 'pos' pourrais se diriger vers la direction 'way'
	u32 canmove(struct position *testpos, u32 way);

	// fonction qui rempli une structure 'group' depuis 2 positions donn�e en argument
	// renvoi 0 si le groupe est invalide, sinon 1..
	u32 setGroup(struct group*grp, struct position*p1, struct position *p2);

	// fonction qui renvoi 1 si la position donn� fait parti du groupe donn�, sinon renvoi 0..
	u32 isInGroup(struct position *pos, struct group *grp);
	u32 isInGroup(u32 colon, u32 line, struct group *grp);
    u32 getNbPion(u32 *p1, u32 *p2); // retourne le nombre de pion au debut du jeux et le nombre de pions courant (� travers deux pointeur)
    u32 getColonSize(u32 colon);

	// 3 fonctions pour changer les pions/info affich� sur le plateau, donner 0 pour 'rien'
	void setDraw(struct position *pos); // pion selectionn�
	void setDraw(struct group *grp); // groupe selectionn�
	void setDraw(u32 line); // ligne selectionn� (dans la demande de direction)
	void deleteSelection(u32 option); // supprime toute selection affich�e sur le plateau
	
private:
	// fonction calculant la position d'une case adjacente � celle donn� en argument, retourne 0 si pas de case adjacente..
	u32 getAdjacentCase(u32 colon, u32 line, u32 way, struct position *pos);
	
	// la meme, mais la case en argument est donn� dans la stucture position de sortie..
	u32 getAdjacentCase(u32 way, struct position *pos);

	// la meme avec deux structure pos differente, une d'entree et une de sortie..
	u32 getAdjacentCase(u32 way, struct position *pos_in, struct position *pos_out);
	
	// fonction retournant le nombre de pion consecutif de la meme couleur,
	//	sur la meme direction, en partant de la position pos,
	// la valeur ecrite dans endValue corespond a ce qui est trouv� � la fin de la chaine
	// -> 0:rien,  1:fin de plateau,  2:pion du joueur adverse (valeur defini dans define.h)
	// la structure position sera fill� avec la derniere position valide
	// !! le nom est recursif, mais pas la fonction ;)
	u32 recursiveScanCase(struct position *pos, u32 way, u32*endValue, struct position*lastPos);

	void recursiveMove(struct position *pos, u32 way, u32 value);

	// fonction testant si deux position sont �gales, retourne 0 ou 1..
	u32 isSame(struct position *p1, struct position *p2);

	// fonction renvoyant le sens inverse du sens donn�, si invalide renvoi 0
	u32 getWayInvert(u32 way);

    console shell; // instance de la classe console, utilis� pour l'affichage du plateau
    log_file *abalog; // instance de la classe gerant le log

	u32 taille;			// taille du plateau, le jeu de base � une taille de 4, le nombre de colone est calcul� avec (taille*2)+1
	u32 nbPion[3];		// nombre de pion courant pour chaque joueur, la 3eme case conntient le nombre de pion au depart
	u32 ** colon;		// tableau contenant les colones
	u32 *  colonSize;	// tableau contenant la taille de chaque colone
	
	struct group selection; // selection actuelle (mis a jour par la classe abalone, declar� ici pour etre utilis� par la fonction d'affichage)
	struct position selected; // position actuelle du pion selectionn�, ici pour les meme raison que le groupe selection
	u32 selectedLine; // ligne selectionn�, utilis� dans l'affichage pour le choix de la colone
};

#endif
