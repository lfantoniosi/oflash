#include "stdio.h"
#include "bdos.h"

#define INITXT	0x006C

#define EXPTBL	0xFCC1
#define CALSLT	0x001C

uint VAddr = 0;

uint CsrX = 0;
uint CsrY = 0;

__at(0xF3B0) uchar LINLEN;
__at(0xF3AE) uchar LINL40;
__at(0xf3dc) uchar CSRX;
__at(0xf3dd) uchar CSRY;

void iniTxt() __naked
{
	LINLEN = 80;
	LINL40 = 80;
	CsrX = 0;
	CsrY = 0;
	VAddr = 0;
	__asm
	push	ix
	ld		ix,#INITXT
	ld		iy,(EXPTBL-1)
	call	CALSLT
	di
	pop		ix
	ret
	__endasm;	
}

int putchar(int c) 
{
	if (c >= 0)
		_putch((char)c);
	return c;
}

int getchar()
{
	return 0;
}

bool is_key_pressed()
{
	uchar row;
	for(row = 0; row < 11; row++)
	{
		PPIC = (PPIC & 0xf0) | row;
		if (PPIB != 255) return TRUE;
	}
	return FALSE;
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

			if (c > 31)
			{
				VAddr = CsrX + (CsrY << 6) + (CsrY << 4);
				__asm
				call	___sdcc_enter_ix
				ld		a,(_VAddr)
				out		(#0x99),a
				ld		a,(_VAddr+1)
				and		a,#0x3f
				or		a,#0x40
				out		(#0x99),a
				ld		a, 4 (ix)
				out		(#0x98),a
				ld		sp, ix
				pop		ix
				__endasm;		
				CsrX++;
			}	
			break;
	}
	
	if (CsrX > 79)
	{
		CsrX = 0;
		CsrY++;
	}

	if (CsrY > 23)
	{
		CsrY = 0;
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
		_putch(*s++);
}

uint getXY()
{
	return CsrX << 8 | CsrY;
}

void setXY(uint xy)
{
	CsrY = (uchar)(xy & 0x00ff);
	CsrX = (uchar)(xy >> 8);
}