
#include "console.h"

void console::setCursorPos(u32 x, u32 y)
{    COORD xy; xy.X = x; xy.Y = y;
     SetConsoleCursorPosition(shell_wnd,xy);     
}

void console::setTextColor(u32 color)
{    SetConsoleTextAttribute(shell_wnd,color);
}

void console::printc(u32 color, const char * format, ...) // colored printf
{    char string[1024]; // 1ko buffer
     va_list va;
     va_start(va, format);
     wvsprintf(string,format,va);
     SetConsoleTextAttribute(shell_wnd,color);
     printf(string);
     setTextColor(BLANC);
}

void console::fullScreen(void)
{    keybd_event(VK_MENU,0x38,0,0);//Simulation appuie sur Alt
     keybd_event(VK_RETURN,0x1c,0,0);// Simulation appuie Entrée
     keybd_event(VK_RETURN,0x1c,KEYEVENTF_KEYUP,0);//Relache Entrée
     keybd_event(VK_MENU,0x38,KEYEVENTF_KEYUP,0);//relache Alt
}

console::console(void)
{    shell_wnd = GetStdHandle (STD_OUTPUT_HANDLE);                 
}

void console::flush(void)
{ int ch; while((ch = getchar()) != '\n' && ch != EOF); // flush input..
}
