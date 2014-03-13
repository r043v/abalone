
#include "abalone.h"

abalone::abalone(u32 taillePlateau)
{	//printlr("abalone : constructeur alternatif");
    this->game = 0;
	initAndPlay(taillePlateau);
}

abalone::abalone(void)
{	//printlr("abalone : constructeur standard");
    this->game = 0;
	initAndPlay(4); // init avec la taille de plateau par default
}

abalone::~abalone(void)
{	delete this->game;
}

void abalone::draw(void)
{	this->game->draw2cmd();				// affichage du plateau vers la console
}

void abalone::initAndPlay(u32 taillePlateau)
{	if(this->game) delete this->game;
    this->game = new plateau(taillePlateau);
	joueurActif = 0;
	this->taillePlateau = taillePlateau;
	play();
}

void abalone::changerJoueurActif(void)
{    joueurActif ^= 1;
     game->deleteSelection(0);
}

void abalone::drawPlayerInfoAndArray(void)
{    u32 playerColor[2] = { PLAYER_1_COLOR,PLAYER_2_COLOR };
     system("cls");
     shell.setCursorPos(3,0);
     u32 nbPion[3];
     nbPion[2] = this->game->getNbPion(&(nbPion[0]),&(nbPion[1]) );
     shell.printc(JAUNE,".. abalone ..\n");
     printf("-----------------------------------------\n");
     printf("    pions [");
     shell.printc(playerColor[0],"%i",6-(nbPion[2]-nbPion[0]));
     printf("|");
     shell.printc(playerColor[1],"%i",6-(nbPion[2]-nbPion[1]));
     printf("] ... joueur actif : ");//,nbPion[2]);
     shell.printc(playerColor[joueurActif],"[%i]",joueurActif+1);
     printf("\n-----------------------------------------\n\n\n");
     this->game->draw2cmd();
     printf("\n\n-----------------------------------------\n\n");
}

u32 abalone::askPosition(struct position *pos, const char *txt)
{ u32 bad = 0; u32 x, y;
   do{ // demande une position au joueur.
      do
      {   drawPlayerInfoAndArray();
          if(bad==1) shell.printc(ROUGE," ** ligne %i non valide! **\n",y-1);
          else if(bad == 255)
                     shell.printc(ROUGE," ** position non valide! **\n");
          bad=1; y=255;
          shell.printc(JAUNE,txt); printf("entrer la ligne [1-%i], 0:retour\n",taillePlateau*2+1);
          scanf("%u",&y); shell.flush();
          if(!y) return 0; else y--;
      } while(y > taillePlateau*2);
           
      bad = 0;
      game->setDraw(y);
      do
      {   
          drawPlayerInfoAndArray();
          if(bad) shell.printc(ROUGE," ** colone %i non valide! **\n",x-1);
          bad=1; x=255;
          shell.printc(JAUNE,txt); printf("ligne %i, entrer la colone [1-%i], 0:retour\n",y+1,game->getColonSize(y));
          scanf("%u",&x); shell.flush();
          if(!x) return 0; else x--;
      } while(x >= game->getColonSize(y));
      
      bad = 255;
      
      pos->colon = y; // save de x dans line et y dans colon (90°)
      pos->line  = x;
    } while(!game->isPosValid(pos));
    return 1;
}

u32 abalone::askWay(const char *txt)
{   u32 way,bad=0;
    do{ drawPlayerInfoAndArray();
        if(bad) shell.printc(ROUGE," ** direction non valide! **\n");
        bad=1; way=255;
        shell.printc(JAUNE,txt); printf("\n 1 2       0:retour\n6   3\n 5 4\n\n");
        //char reponse = ' '; scanf("%c",&reponse);
        scanf("%u",&way); shell.flush();
        if(!way) return 0xff;
        //if(reponse == 'x') return 0xff; // revenir au menu
         //else way -= '0'; // ascii -> int
        
/*     
changement de repere, entre l'affichage et le moteur du jeux
    1                  3
 32   2      ->       2 4
 16   4               1 5
    8                  6
    */
        
        if(way == 1) return 16;
        if(way == 2) return 32;
        if(way == 3) return 1;
        if(way == 4) return 2;
        if(way == 5) return 4;
        if(way == 6) return 8;
      } while(1);
}

// ihm
void abalone::getInfoFromPlayer(void)
{    u32 waychoose; // variable utilisée pour tester la valeur de retour de la fonction de demande de direction
     
     // destruction de la validité de la selection precedente, selection, affichée sur l'aire de jeux..
     game->deleteSelection(0);
         
      char whatMove; // 0:pion, 1:groupe
      
      // demande quoi deplacer
      do
      {  whatMove = ' ';
         drawPlayerInfoAndArray(); // affichage stat et jeux
         // demande ce que l'on doit deplacer, un pion ou un groupe..
         shell.printc(JAUNE,"- que deplacer ? - ");
         printf("p: un pion, g: un groupe, x: passer son tour\n");
         scanf("%c",&whatMove);
         shell.flush();
      } while(whatMove != 'p' && whatMove != 'g' && whatMove != 'x');

     switch(whatMove)
     { 
       case 'p': // deplacement d'un pion
            struct position pos; pos.colon = pos.line = 0xff;
            
            while(1)
            { if(!askPosition(&pos,"- coordonnees du pion - ")) return;
              if(game->get(&pos) == joueurActif+1) break;
               else
              {   shell.printc(ROUGE,"\nvous ne pouvez choisir que l'un de vos pions!\n");
                  system("pause");
              }
            };
            
            // pour que le pion selectionné soit affiché visuellement
            game->deleteSelection(GROUPINFO);
            game->deleteSelection(LINEINFO);
            game->setDraw(&pos);

            // demande de la direction
            waychoose = askWay("- direction du deplacement -");
            if(waychoose == 0xff) return; // le player a choisi anuller
            
            // deplacement du pion..
            if(!game->move(&pos,waychoose))
            { shell.printc(ROUGE,"\ndeplacement impossible !\n");
              system("PAUSE");                             
            } else changerJoueurActif();
       break;
       
       case 'g': // deplacement d'un groupe
           struct position start, end;
           struct group grp;
           do
           { while(1)
             {
                // demande les coordonnees de debut du groupe
                while(1)
                { if(!askPosition(&start,"- coordonnees de debut du groupe - ")) return;
                  if(game->get(&start) == joueurActif+1) break;
                   else
                  {   shell.printc(ROUGE,"\nvous ne pouvez choisir que l'un de vos pions!\n");
                      system("pause");
                  }
                };
                
                // pour que le pion selectionné soit affiché visuellement
                game->deleteSelection(GROUPINFO);
                game->deleteSelection(LINEINFO);
                game->setDraw(&start);
                
                // demande les coordonnees de fin du groupe
               while(1)
                { if(!askPosition(&end,"- coordonnees de fin du groupe - ")) return;
                  if(game->get(&end) == joueurActif+1) break;
                   else
                  {   shell.printc(ROUGE,"\nvous ne pouvez choisir que l'un de vos pions!\n");
                      system("pause");
                  }
                };

                // definition du groupe courant (pour l'affichage)
                if(!game->setGroup(&grp,&start,&end))
                { shell.printc(ROUGE,"\ngroupe non valide!\n");
                  game->deleteSelection(0);
                  //shell.printc(ROUGE,"%i.%i -> %i.%i\n",start.colon,start.line,end.colon,end.line);
                  system("pause");                                                
                }
               else
                {  game->deleteSelection(SELECTINFO);
                   game->deleteSelection(LINEINFO);
                   game->setDraw(&grp);
                   break;
                } 
              }; // fin du 'while' du debut

              // demande de la direction de deplacement du groupe
              u32 way = askWay("- direction du deplacement -");
              if(way == 0xff) return;

              if(!game->move(&grp,way))
              {  game->deleteSelection(0);
                 shell.printc(ROUGE,"\ndeplacement impossible!\n");
                 system("pause");                             
              } else break; // si le deplacement est possible, on quitte la boucle infinie (le while 1)
           } while(1); // fin du 'do' du debut
           changerJoueurActif();
       break;
       
       case 'x':
            changerJoueurActif(); // sauter son tour
       break;
     };
}

u32 abalone::isFinish(void)
{   u32 nbPion[3];
    nbPion[2] = this->game->getNbPion(&(nbPion[0]),&(nbPion[1]) );
    if(nbPion[2]-nbPion[0] >= 6) return 2; // player 2 win
    if(nbPion[2]-nbPion[1] >= 6) return 1; // player 1 win
    return 0;
}

void abalone::play(void)
{    // affichage du titre
     shell.printc(JAUNE,"\n\n\t\tabalone!\n\n");    
     shell.printc(VERT,"           _____      _               _____ _     _ \n          |  ___|    (_)             |_   _| |   | |\n          | |__ _ __  _  ___  _   _    | | | |_  | |\n          |  __| '_ \\| |/ _ \\| | | |   | | | __| | |\n          | |__| | | | | (_) | |_| |  _| |_| |_  |_|\n          \\____/_| |_| |\\___/ \\__, |  \\___/ \\__| (_)\n                    _/ |       __/ |                \n                   |__/       |___/\n\n\n\n\n");
     system("pause");
    
     // lancement du jeux
     while(!isFinish())
       getInfoFromPlayer(); // fait jouer la partie
    
     // partie terminée, affichage du gagnant
     drawPlayerInfoAndArray(); // affichage stat et jeux
     shell.printc(JAUNE,"\n\n     ** partie terminee **");
     shell.printc(JAUNE,"\n          gagnant : joueur %i !",isFinish());
     shell.printc(JAUNE,"\n\n\n     merci d'avoir joue :D\n\n");
     system("pause");
     // and bye bye :)
}
