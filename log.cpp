
#include "log.h"

void log_file::log(const char * info, int lineReturn)
{	const char * openMethod = "a" ;
	if(!info)    openMethod = "w" ;

	FILE *f = fopen(path,openMethod);
	if(info)
		{	if(lineReturn)	fprintf(f,"%s\n",info);
			 else			fprintf(f,"%s",info);
		}
	fclose(f);
}

// a printf who write in the log
void log_file::printl(const char * format, ...)
{	va_list va;
	va_start(va, format);
	wvsprintf(dbg,format,va);
    log(dbg,0);
}

// the same, but who insert a line return
void log_file::printlr(const char * format, ...)
{	va_list va;
	va_start(va, format);
	wvsprintf(dbg,format,va);
	log(dbg,1);
}

// constructor
log_file::log_file(const char * path)
{  this->path = path;
   log(0,0);      
}

// insert a line return
void log_file::lr(void)
{    log("",1);  
}
