
#ifndef _console_
#define _console_

#include "define.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>

class console
{ private:
      HANDLE shell_wnd;
  public:
      void fullScreen(void); // met la console en plein ecran
      void printc(u32 color, const char * format, ...); // colored printf
      void setTextColor(u32 color); // definition de la couleur du texte
      void setCursorPos(u32 x, u32 y); // definition de la position du curseur
      void flush(void); // pour effacer les entree clavier en cas de scanf non valide
      console(void);
};

#endif
