#include "stdio.h"
#include "bdos.h"


#define INITXT	0x006C
#define	LINLEN 	0xF3B0
#deinfe CSRX	0xF3DC
#deinfe CSRY	0xF3DD
#define EXPTBL	0xFCC1
#define CALSLT	0x001C

uint VAddr = 0;

__at(CSRX) uchar CsrX;
__at(CSRY) uchar CsrY;
__at(LINLEN) uchar 	LinLen;

void iniTxt() __naked
{
	LinLen = 80;
	CsrX = 0;
	CsrY = 0;
	VAddr = 0;
	__asm
	ld		ix,#INITXT
	ld		iy,(EXPTBL-1)
	call	CALSLT
	di
	ret
	__endasm;	
}

int putchar(int c) 
{
	if (c >= 0)
		_putch((char)c);
	return c;
}

int getchar() __naked
{
	__asm
	ret
	__endasm;
}

char is_key_pressed() __naked
{
	__asm
	ret
	__endasm;	
}

void _putch(uchar c) __naked
{
	switch(c)
	{
		case	0x0a:
			CsrY++;
			break;
		case	0x0d:
			CsrX = 0;
			break;
		default:
			break;
	}
	
	if (c > 31)
	{
		VAddr = (uint)CsrX + ((uint)CsrY << 5) + ((uint)CsrY << 4);
		__asm
		call	___sdcc_enter_ix
		ld		a,(VAddr)
		out		(099h),a
		ld		a,(VAddr+1)
		and		#03fh
		or		#040h
		out		(099h),a
		ld		a, 4 (ix)
		ld		(098h),a
		ld		sp, ix
		pop		ix
		__endasm;		
		CsrX++;
	}	
	
	if (CsrX > 79)
	{
		CsrX = 0;
		CsrY++;
		
		if (CsrY > 23)
			Csry = 0;
	}

	return;
}

char _getch() __naked
{
	__asm
	ret
	__endasm;
}


void fputs(const char *s)
{
	while(*s != NULL)
}		_putch(*s++);
