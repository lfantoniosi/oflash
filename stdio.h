#ifndef _STDLIB_H
#define _STDLIB_H_

#include "types.h"

void _putch(uchar c);
char _getch();
bool is_key_pressed();
void fputs(const char *s);
uint getXY();
void setXY(uint xy);
void iniTxt();


__at(0xFCC1) uchar EXPTBL[4];
__at(0xFFFF) volatile uchar SLTREG;
__sfr __at (0xA8) PPIA;
__sfr __at (0xA9) PPIB;
__sfr __at (0xAA) PPIC;

__sfr __at (0xFC) MM_0000;
__sfr __at (0xFD) MM_4000;
__sfr __at (0xFE) MM_8000;
__sfr __at (0xFF) MM_C000;

#define ENABLE_INT   \
         __asm       \
            ei       \
        __endasm

#define DISABLE_INT  \
         __asm       \
            di       \
        __endasm

#define HALT         \
         __asm       \
            halt     \
        __endasm


#endif