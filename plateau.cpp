
#include "plateau.h"

plateau::plateau(u32 taille)
{	
	u32 colonSize = 1+taille*2; // calcul de la largeur de la colone centrale
	this->taille = taille;

    abalog = new log_file("abalone.log");

	#ifdef DEBUG
		abalog->printlr("creation du plateau..\ntaille de jeux : %i",taille);
		abalog->printlr("nombre de colones : %i",colonSize);
	#endif

	colon = (u32**)malloc(sizeof(u32)*colonSize); // affectation de la liste contenant les colones
	this->colonSize = (u32*)malloc(sizeof(u32)*colonSize); // affectation de la liste contenant la taille des colones
	u32 colonSizeCount=0; // variable pour parcourir plus simplement la liste des taille de colone
	
	// allocation et remplissage des colones de gauche
	for(u32 n=0 ; n<taille ; n++)
	{	u32 size = (colonSize-taille+n); // taille de la colone
		this->colonSize[colonSizeCount++] = size; // sauvegarde de la taille..
		#ifdef DEBUG
			abalog->printlr("colone %i .. taille : %i",n,size); // debug %)
		#endif
		size *= sizeof(u32); // taille*=4 pour la convertir en octet
		
		colon[n] = (u32*)malloc(size); // alloc
		memset(colon[n],0,size); // init à 0 (rien)
	};

	// alloc et affectation de la colone centrale et de celles de droite
	for(u32 n=0 ; n <= taille ; n++)
	{	u32 size = (colonSize-n);
		this->colonSize[colonSizeCount++] = size;
		#ifdef DEBUG
			abalog->printlr("colone %i .. taille %i",n+taille,size);
		#endif
		size *= sizeof(u32);
		
		colon[n+taille] = (u32*)malloc(size);
		memset(colon[n+taille],0,size);
	};

	// positionement des pions, remplissage des 2 premieres colones,
	// et de la moitié arrondie a l'inferieur de la 3 eme.
	// ce qui fait 14 pions pour la taille de plateau standard (4)

    nbPion[0] = nbPion[1] = nbPion[2] = 0;

	// player 1
	u32 size = this->colonSize[0]; // taille colone 1
	for(u32 n=0;n<size;n++)
	{	(colon[0])[n] = 1; // fill à 1 (player 1)
		(*nbPion)++;		// incrementation du compteur de pions...
	};
	size = this->colonSize[1]; // taille colone 2 ..
	for(u32 n=0;n<size;n++)
	{	(colon[1])[n] = 1; // fill à 1
		(*nbPion)++;
	};
	size = this->colonSize[2]; // colone 3
	u32 start = 1 + (size>>2); size>>=1; size++; // fill à 1 du centre de la colone 3, taille arrondie a l'inferieur
	for(u32 n=start;n<=size;n++)
	{	(colon[2])[n] = 1;
		(*nbPion)++;
	};

	// player 2
	u32 id = colonSize-1;
	size = this->colonSize[id];
	for(u32 n=0;n<size;n++)
	{	(colon[id])[n] = 2;
		nbPion[1]++;
	};
	id--;
	size = this->colonSize[id];
	for(u32 n=0;n<size;n++)
	{	(colon[id])[n] = 2;
		nbPion[1]++;
	};
	id--;
	size = this->colonSize[id];
	start = 1 + (size>>2); size>>=1; size++;
	for(u32 n=start;n<=size;n++)
	{	(colon[id])[n-(taille&1)] = 2; // taille&1 -> pour faire 'symetrie' dans le cas ou les pions de la 3eme colone ne sont pas centrés, code valide pour de petites tailles
		nbPion[1]++;
	};
	nbPion[2] = nbPion[0]; // nbPion[2] contiendra le nombre de pion au depart
	abalog->printlr("nombre de pions -> %u et %u   sur %u\n",nbPion[0],nbPion[1],nbPion[2]);
}

void plateau::deleteSelection(u32 option)
{    if(!option) option = 0xffffffff;
     if(option & LINEINFO)   selectedLine=0;
     if(option & SELECTINFO) selected.line = selected.colon = 0xff;
     if(option & GROUPINFO)  selection.size = 0xff;
}

void plateau::draw2cmd(void) // affichage vers la console
{	u32 colonSize = 1+taille*2;
    u32 playerColor[2] = { PLAYER_1_COLOR,PLAYER_2_COLOR };
    u32 infoColor[2]   = { INFO_COLOR_1,INFO_COLOR_2 };
//    printf("selected line %i\n",selectedLine);

	// affichage tourné de 90° par simplicitée (le jeux est géré par colone et la console par ligne...)
	for(u32 x=0 ; x < colonSize ; x++)
	{	u32 size = this->colonSize[x];//colonSizeCount++]; // taille de la colone courante

       if(selectedLine && selectedLine-1 == x) 
       {    if(selectedLine)
            {   u32 size = this->colonSize[selectedLine-1];
                printf("   ");
                for(u32 y=!(taille&1) ; y<colonSize-size ; y++) printf(" ");
                
                for(u32 y=0;y<size;y++)
                { u32 v = (colon[x])[y];
                  if(v) shell.printc(infoColor[y&1],"%2u",y+1);
                  else printf(" .");
                }
                printf("\n"); 
            }
             
          shell.printc(JAUNE,"%2u ",x+1);
       }
       else printf("%2u ",x+1);

		for(u32 y=0 ; y<colonSize-size ; y++) printf(" "); // insertion d'espace..
		for(u32 y=0;y<size;y++)
		{	u32 v = (colon[x])[y];
			if(!v)
                shell.printc(JAUNE,". "); // case vide on affiche un point
			else
            {   char pionDraw = 'o'; // char du pion normal
                if(isInGroup(x,y,&selection)) pionDraw = '*'; // ,du groupe selectionné
                 else
                { struct position pos; pos.line=y, pos.colon=x;
                  if(isSame(&pos,&selected)) pionDraw = '*'; // et du pion unique selectionné                     
                }
                shell.printc(playerColor[v-1],"%c ",pionDraw); // ou un rond coloré de la couleur du pion
            }
		};
		printf("\n"); // retour à la ligne
	};
}

u32 plateau::getColonSize(u32 colon)
{      if(!colonSize || colon>(taille*2)+1) return 0xff;    
       return colonSize[colon];
}

void plateau::draw2log(void) // affichage vers le log..
{	u32 colonSize = 1+taille*2;
	u32 colonSizeCount = 0;
	abalog->printlr("\ngame array ..\n");

	// affichage tourné de 90° par simplicitée (le jeux est géré par colone et la console par ligne...)
	for(u32 x=0 ; x < colonSize ; x++)
	{	u32 size = this->colonSize[colonSizeCount++];
		for(u32 y=0 ; y<colonSize-size ; y++) abalog->printl(" ");
		
		for(u32 y=0;y<size;y++)
		{	u32 v = (colon[x])[y];
			if(!v) abalog->printl(". ");
			else	abalog->printl("%i ",(colon[x])[y]);
		};

		abalog->printlr("");
	};
}

plateau::~plateau(void)
{	if(!this->colon) return;
	u32 colonNumber = 1+taille*2;
	for(u32 n=0 ; n<colonNumber ; n++)
		if(this->colon[n]) free(this->colon[n]);
	free(this->colon);
}

u32 plateau::isPosValid(u32 colon, u32 line)
{	u32 colonNumber = this->taille*2 + 1;
	if(colon >= colonNumber) return 0;
	u32 colonSize = this->colonSize[colon];
	if(line >= colonSize) return 0;

	return 1;
}

u32 plateau::set(u32 colon, u32 line, u32 value)
{	if(!isPosValid(colon,line)) return 0xff;
	(this->colon[colon])[line] = value;
	return 1;
}

u32 plateau::get(u32 colon, u32 line)
{	if(!isPosValid(colon,line)) return 0xff;
	return (this->colon[colon])[line];
}

u32 plateau::isPosValid(struct position *pos)
{	if(!pos) return 0;// abalog->printlr(" ** isPosValid : pointer empty ..");
	return isPosValid(pos->colon,pos->line);
}

u32 plateau::set(struct position *pos, u32 value)
{	return set(pos->colon,pos->line,value);
}

u32 plateau::get(struct position *pos)
{	return get(pos->colon,pos->line);
}

u32 plateau::getAdjacentCase(u32 colon, u32 line, u32 way, struct position *pos)
{  	
	abalog->printlr(" * enter getAdjacent with %u.%u",colon,line);
	if(!way || !pos || !isPosValid(colon,line)) return 0; // input args sanity check

	pos->colon = colon;
	pos->line  = line;
	
	// si pas de changement de colone, travail direct sur les lignes
	if(way==UP || way==DOWN)
	{	if(way == UP)	pos->line++;
		 else			pos->line--;
	}
  else // sinon, travail sur les lignes et colones
	{	u32 colonPos = AUCENTRE; // variable definissant la position de la colone courante par rapport à la centrale

		if(colon < this->taille) colonPos=AGAUCHE; // la colone courante est à gauche de la centrale
			else
		if(colon > this->taille) colonPos=ADROITE; // la colone est a droite de la centrale
	
		// switch, pour bouger les positions suivant la direction..
		switch(way)
		{
			case UPLEFT:
				pos->colon--;
				if(colonPos == ADROITE) pos->line++;
			break;
			
			case UPRIGHT:
				pos->colon++;
				if(colonPos == AGAUCHE) pos->line++;
			break;
			
			case DOWNLEFT:
				pos->colon--;
				if(colonPos == AGAUCHE || colonPos == AUCENTRE) pos->line--;
			break;
			
			case DOWNRIGHT:
				pos->colon++;
				if(colonPos == ADROITE || colonPos == AUCENTRE) pos->line--;
			break;
		};
	}

	// la nouvelle position est calculée, test de sa validitée
	// note : les valeur etants non signée, un -- fait sur une valeur 0 donnera 0xffffffff,
	//	soit, une valeur plus haute que la taille max de la colone ou ligne...

	#ifdef DEBUG
  		abalog->printl(" ** new pos for %i.%i with way %i\t: %i.%i\t.. ",colon,line,way,pos->colon,pos->line);
	#endif

	if(!isPosValid(pos))
		{
			#ifdef DEBUG
				abalog->printlr("bad!");
			#endif

			return 0;
		} // retourne 0 si la position n'est pas sur la grille..

	#ifdef DEBUG
		abalog->printlr("valid!");
	#endif

	return 1;
}

u32 plateau::getAdjacentCase(u32 way, struct position *pos)
{	u32 x = pos->colon, y = pos->line;
	return getAdjacentCase(x,y,way,pos);
}

u32 plateau::getAdjacentCase(u32 way, struct position *pos_in, struct position *pos_out)
{	return getAdjacentCase(pos_in->colon,pos_in->line,way,pos_out);
}

u32 plateau::isAdjacent(struct position *p1, struct position *p2)
{	struct position tmp;

	if(!isPosValid(p1) || !isPosValid(p2)) return 0;

	for(u32 way=1;way<64;way<<=1) // pour toute les directions..
	{	tmp.colon = p1->colon;
		tmp.line  = p1->line;

		if(getAdjacentCase(way,&tmp)) // capture de la position de la case adjacente
			if(isSame(&tmp,p2)) return way; // si meme coordonées, retourne la direction
	};

	return 0; // les 2 positions ne sont pas adjacentes, return 0
}

u32 plateau::isSame(struct position *p1, struct position *p2)
{	if(p1->colon == p2->colon)
		if(p1->line == p2->line)
			return 1;
	return 0;
}

u32 plateau::move(struct position *src, struct position *dst)
{	u32 way = isAdjacent(src,dst);
	if(!way) return 0;
	return move(src,way);
}

// fonction gerant le deplacement d'un seul pion
u32 plateau::move(struct position *pos, u32 way)
{	u32 value = get(pos); // capture de la valeur de la position de depart
	if(value == 0xff || !value || value > 2) return 0; // case vide, non valide ou non sur le plateau..
	struct position start_pos;
	start_pos.colon = pos->colon;
	start_pos.line  = pos->line;
	if(!getAdjacentCase(way,pos)) return 0; // return si la case de destination non sur le plateau..
	u32 dvalue = get(pos); // reutilisation de pos, car le getAdjacent la actualisé.. (d'ou la sauvegarde plus haut..)
	if(dvalue == 0xff || dvalue > 2) return 0; // case de destination non sur le plateau (ou valeur invalide), suicide impossible -> return 0..

    #ifdef DEBUG
       abalog->printlr(" * deplacement d'un pion");
    #endif

	if(!dvalue && dvalue<3) // destination vide, deplacement direct du pion..
	{	set(&start_pos,0);
		set(pos,value);
        #ifdef DEBUG
		       abalog->printlr("\tdestination vide, deplacement direct du pion..");
        #endif
		return 1;
	}
#ifdef unPionPeuEnDeplacerUnAutre
	// case de destination pleine..
	if(dvalue == value) // si meme valeur de pion que celui a deplacer..
		return 0; // utiliser le deplacement par groupe.......
	else
	{	// dans ce mode on ne pourra deplacer qu'un seul pion adverse

		struct position third_pos;
		third_pos.colon = pos->colon;
		third_pos.line  = pos->line;

		if(!getAdjacentCase(way,&third_pos))
		{ // la case deriere le pion adverse n'est pas sur le plateau!
			set(&start_pos,0);
			set(pos,value);
			nbPion[dvalue-1] -= 1;
            #ifdef DEBUG
                   abalog->printlr("\tpousse un pion adverse dehors!");
            #endif
			return 1;
		}
		  else
		{ // la case deriere le pion adverse est sur le plateau et vide, deplacement ok
			u32 third_value = get(&third_pos);
			if(third_value > 0) return 0; // test case vide
			set(&start_pos,0);		// case de depart du pion courant, mise a 0
			set(pos,value);			// case de destination du pion courant, mise à la valeur du pion
			set(&third_pos,dvalue); // case deriere le pion adverse, mise à la valeur du pion adverse..
            #ifdef DEBUG
            	//abalog->printlr("pousse un pion adverse..");
            	abalog->printlr("\tpousse un pion adverse..\n * start pos : %i.%i, dest pos : %i.%i, third pos : %i.%i",start_pos.colon,start_pos.line,pos->colon,pos->line,third_pos.colon,third_pos.line);
            #endif
			return 1;
		}
	}
#endif

	return 0;
}

// copie de la fonction move, mais qui ne deplace pas les pion,
// se contente de dire si le deplacement se serais bien passé
u32 plateau::canmove(struct position *testpos, u32 way)
{	struct position copypos, *pos;
    pos = &copypos;
    copyPos(testpos,pos);
    u32 value = get(pos); // capture de la valeur de la position de depart
	if(value == 0xff || !value || value > 2) return 0; // case vide, non valide ou non sur le plateau..
	struct position start_pos;
	start_pos.colon = pos->colon;
	start_pos.line  = pos->line;
	if(!getAdjacentCase(way,pos)) return 0; // return si la case de destination non sur le plateau..
	u32 dvalue = get(pos); // reutilisation de pos, car le getAdjacent la actualisé.. (d'ou la sauvegarde plus haut..)
	if(dvalue == 0xff || dvalue > 2) return 0; // case de destination non sur le plateau (ou valeur invalide), suicide impossible -> return 0..

	if(!dvalue && dvalue<3) // destination vide, deplacement direct du pion..
		return 1;

#ifdef unPionPeuEnDeplacerUnAutre
	// case de destination pleine..
	if(dvalue == value) // si meme valeur de pion que celui a deplacer..
		return 0; // utiliser le deplacement par groupe.......
	else
	{	// dans ce mode on ne pourra deplacer qu'un seul pion adverse

		struct position third_pos;
		third_pos.colon = pos->colon;
		third_pos.line  = pos->line;

		if(!getAdjacentCase(way,&third_pos))
			return 1;
		  else
		{ // la case deriere le pion adverse est sur le plateau et vide, deplacement ok
			u32 third_value = get(&third_pos);
			if(third_value > 0) return 0; // test case vide
			return 1;
		}
	}
#endif

	return 0;
}


u32 plateau::getWayInvert(u32 way)
{	switch(way) // pas envi de taper les nom de constante %)
		{	case 1 : return 8;	break;
			case 2 : return 16;	break;
			case 4 : return 32;	break;
			case 8 : return 1;	break;
			case 16: return 2;	break;
			case 32: return 4;	break;
		};

	return 0;
}

u32 plateau::move(struct group *grp, struct position *pos)
{	u32 way = isAdjacent(&(grp->start_pos),pos);
	if(!way) way = isAdjacent(&(grp->end_pos),pos);
	if(!way)
	{	struct position intermediaire;
		getAdjacentCase(grp->way,&(grp->start_pos),&intermediaire);
		way = isAdjacent(&(grp->end_pos),pos);
	}

	if(!way) return 0;
	return move(grp,way);
}

u32 plateau::recursiveScanCase(struct position *pos, u32 way, u32*endValue, struct position*lastPos)
{	if(!isPosValid(pos))	return 0;
	u32 value = get(pos);	if(!value) return 0;
	u32 consecutive = 1;

	struct position last;	copyPos(pos,(&last));

	while(1)
	{	if(!getAdjacentCase(way,&last,lastPos)) // get next one ...
		{	 // prochaine position en dehors de l'aire de jeux
			*endValue = END_OF_ARRAY;
			copyPos((&last),lastPos);
			return consecutive;
		}

		u32 newValue = get(lastPos); // capture valeur
		
		if(!newValue) // case vide
		{	*endValue = NOTHING;
			return consecutive;
		}
		
		if(newValue != value) // un pion non adverse
		{	*endValue = OUR_PION;
			return consecutive;
		}

		consecutive++;
		copyPos(lastPos,(&last));
	};
}

void plateau::recursiveMove(struct position *pos, u32 way, u32 value)
{	struct position next;
	u32 currentValue = get(pos);
	if(currentValue == 0xff) return; // la position donné est non valide -> return ..
	
	if(!currentValue) // stop de la recursion si la case actuelle est vide
	{	set(pos,value);
		return;
	}

	if(getAdjacentCase(way,pos,&next))
		recursiveMove(&next,way,currentValue);
	else	{	// stop de la recursion si au bord du plateau..
                nbPion[currentValue-1] -= 1; // destruction du dernier pion
				set(pos,value);
				return;
			}
	set(pos,value);
}

u32 plateau::move(struct group *grp, u32 way)
{	if(!grp || !way) return 0; // sanity check...
	if(grp->size < 2 || grp->size > 3) return 0;
#ifdef DEBUG
	abalog->printl("** deplacement d'un groupe ** ");
	debugGroup(grp);
#endif
	u32 iway = getWayInvert(grp->way);
	if(way == grp->way || way == iway) // si le deplacement est dans le meme 'vecteur' que le groupe
	{
        #ifdef DEBUG
        	abalog->printlr("\tdeplacement dans le sens du groupe");
        #endif
		if(way == iway) // par simplicité, inversion du groupe
		{
            #ifdef DEBUG
            	debugGroup(grp);
                abalog->printl("\tinversion du groupe -> ");
                debugGroup(grp);
            #endif
			flipPos((&(grp->start_pos)),(&(grp->end_pos)));
			grp->way = iway;
		}

		// get de la case de destination du groupe
		struct position out;	u32 value;

		if(!getAdjacentCase(way,&(grp->end_pos),&out))
        {
            #ifdef DEBUG
               abalog->printlr("case de destination en dehors du plateau");
            #endif
            return 0; // si position en dehors du plateau, return 0
        }
		value = get(&out); // lecture du contenu de la case..

        #ifdef DEBUG
               //abalog->printlr("move debug :^) pos %i.%i, valeur %i, valeur du groupe : %i",out.colon,out.line,value,grp->value);
        #endif

		if(value == grp->value) return 0; // on ne peu deplacer ses propres pions à travers un groupe...

		if(!value) // la case est vide, parfait :'D
		{	move(&(grp->end_pos),way);
			if(grp->size == 3) move(&(grp->mid_pos),way);
			move(&(grp->start_pos),way);
            #ifdef DEBUG
                   abalog->printlr("case de destination vide");
            #endif
            return 1;
		}

		// la case contient un pion adverse, notre groupe est il plus grand ?

		u32 endValue = 0;
		struct position lastPos;
		u32 nbFound = recursiveScanCase(&out,way,&endValue,&lastPos); // calcul du nombre de pion adverse..

		if(nbFound > 2) return 0; // groupe adverse trop grand pour etre deplacé ..

		if(grp->size > nbFound) // si la taille de notre groupe est superieure à celle du groupe adverse..
		{	if(endValue != OUR_PION) // et si la fin du groupe adverse est rien ou le bord du plateau..
			{	// alors, le deplacement est possible ^___^
                #ifdef DEBUG
                        abalog->printlr("deplacement possible! endval %i last [%i.%i]",endValue,lastPos.colon,lastPos.line);
                #endif
				recursiveMove(&(grp->start_pos),way,0);
				return 1;
			}
		}

		return 0;
	}
   else
	{	// deplacement du groupe sur un 'vecteur' different de celui ci,
		// gestion séparée du deplacement : pion par pion, utilisation de la fonction standard
        #ifdef DEBUG
               abalog->printlr("\tdeplacement pion par pion");
        #endif
		
        u32 moveCount = 0;

#ifdef unPionBloqueBloqueLesAutre
		// test pour voir si tous les pions peuvent etre deplacés
        moveCount += canmove(&(grp->start_pos),way);
		moveCount += canmove(&(grp->end_pos),way);
		if(grp->size == 3)	moveCount += canmove(&(grp->mid_pos),way);

//        printf("\nmove count %i grpSize %i\n",moveCount,grp->size);
//        system("pause");

		if(moveCount != grp->size) return 0; // si tous les pions ne peuvent se deplacer, return 0
#endif
        moveCount = 0;
        moveCount += move(&(grp->start_pos),way);
        moveCount += move(&(grp->end_pos),way);
        if(grp->size == 3)	moveCount += move(&(grp->mid_pos),way);

        return (moveCount!=0); // si au moins un des pions a pu etre deplacé, return 1
	}

	return 0;
}

u32 plateau::setGroup(struct group*grp, struct position*p1, struct position *p2)
{	if(!grp || !p1 || !p2) return 0; // sanity check..
	if(!isPosValid(p1) || !isPosValid(p2)) return 0; // sanity check..
	
	u32 value = get(p1);
	if(!value || get(p2) != value) return 0; // case vide ou de valeur differentes

	grp->value = value;

	// test pour voir si les positions sont adjacentes -> seulement 2 pions
	u32 way = isAdjacent(p1,p2);
	if(way)
	{	grp->way = way;
		grp->size = 2;
		copyPos(p1,(&(grp->start_pos)));
		copyPos(p2,(&(grp->end_pos)));
//		debugGroup(grp);
		return 1;
	}

	// la taille est de 3 pions, ou le groupe est non valide

	for(way=1;way<64;way<<=1)
	{	struct position intermediaire;

		if(getAdjacentCase(way,p1,&intermediaire))
		{	u32 way2 = isAdjacent(&intermediaire,p2);
			if(way == way2) // groupe valide, 3 pions allignés sur la meme direction
			{	if(get(&intermediaire) != value) return 0; // tout ok, sauf la valeur du pion intermediaire..
				grp->way = way;
				grp->size = 3;
				copyPos(p1, (&(grp->start_pos)) );
				copyPos(p2, (&(grp->end_pos)) );
				copyPos((&intermediaire), (&(grp->mid_pos)) );
//				debugGroup(grp);
				return 1;
			}
		}
	};

	return 0; // groupe non valide..
}

u32 plateau::isInGroup(struct position *pos, struct group *grp)
{	if(!pos || !grp) return 0;
	if(grp->size < 2 || grp->size > 3)	return 0;
	if(isSame(pos,&(grp->start_pos)))	return 1;
	if(isSame(pos,&(grp->end_pos)))		return 1;
	if(grp->size == 3) return isSame(pos,&(grp->mid_pos));
	return 0;
}

u32 plateau::isInGroup(u32 colon, u32 line, struct group *grp)
{	struct position pos; pos.colon = colon; pos.line = line;
	return isInGroup(&pos,grp);
}

u32 plateau::getNbPion(u32 *p1, u32 *p2)
{   if(p1) *p1 = nbPion[0];
    if(p2) *p2 = nbPion[1];
   // abalog->printlr("\n ** pions %i.%i.%i",nbPion[0],nbPion[1],nbPion[2]);
    return nbPion[2];
}

void plateau::setDraw(struct position *pos)
{    if(!isPosValid(pos)) pos=0;
     if(!pos)
     { selected.line = 0xff;
       selected.colon = 0xff;
       return;  
     }
             
     selected.line = pos->line;
     selected.colon = pos->colon;
}

void plateau::setDraw(struct group *grp)
{    if(grp) setGroup(&selection,&(grp->start_pos),&(grp->end_pos));
      else selection.size = 0xff;
}

void plateau::setDraw(u32 line)
{    if(line == 0xff || line > taille*2+1) { selectedLine = 0; return ; }
     selectedLine = line + 1;
}

