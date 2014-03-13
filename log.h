
#ifndef _log_
#define _log_

#include <stdio.h>   // header stdio pour les entrée/sortie de fichier
#include <windows.h> // header windows.h pour le wvsprintf et les va_list

// classe 'log_file', se charge de gerer un fichier de log, pour simplifier le debug

class log_file
{   private:
      const char * path;
      void log(const char * info, int lineReturn);
      char dbg[1024]; // 1ko of work buffer
    public:
      void printl(const char * format, ...); // un printf qui ecrit dans le log
      void printlr(const char * format, ...);// le meme, qui insere egalement un retour à la ligne
      void lr(void); // ajoute un retour à la ligne
      log_file(const char *path); // constructeur, l'argument sera le path du fichier log
};

#endif
