#include <string.h>
#include <ctype.h>

#include "bdos.h"


void bdos() __naked
{
	__asm
	push	ix
	push	iy
	call	BDOS
	pop		iy
	pop		ix
	ret
	__endasm;
}

uint bdos_drv_set(uchar drive) __naked
{
	drive;
	__asm
	call	___sdcc_enter_ix

	ld		e, 4 (ix)
	ld		c,#BDOS_DRV_SET
	call	_bdos
	
	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uchar bdos_drv_get() __naked
{
	__asm
	call	___sdcc_enter_ix

	ld		c,#BDOS_DRV_GET
	call	_bdos
	ld  	l,a

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_open(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_OPEN
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_close(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_CLOSE
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_make(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_MAKE
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

void bdos_f_dmaoff(void* dmaoff) __naked
{
	dmaoff;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_DMAOFF
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_read(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_READ
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_rnd_readout(fcb_t* fcb, uint count) __naked
{
	fcb; count;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		l, 6 (ix)
	ld		h, 7 (ix)

	ld		c,#BDOS_F_READOUT
	call	_bdos

	ld		l,a

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_write(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_WRITE
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uint bdos_f_delete(fcb_t* fcb) __naked
{
	fcb;
	__asm
	call	___sdcc_enter_ix
	ld		e, 4 (ix)
	ld		d, 5 (ix)

	ld		c,#BDOS_F_DELETE
	call	_bdos

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

bool bdos_init_fcb(fcb_t* fcb, const char* filename)
{
	const char *dot, *dr;
	size_t i;
	memset(fcb, 0, sizeof(fcb_t));
	memset(&fcb->F[0], ' ', 11);
	
	dr = strchr(filename, ':');
	if (dr)
	{
		if (dr - filename != 1)
			return FALSE;
		fcb->DR = toupper(dr[0]) - 'A';
		if (fcb->DR > 25) return FALSE;
		filename = dr + 1;
	}
	else fcb->DR = bdos_drv_get();
 
	dot = strchr(filename, '.');
	if (dot)
	{
		memcpy(&fcb->F[0], filename, MIN(dot - filename, 8));
		dot++;
		memcpy(&fcb->T[0], dot, MIN(strlen(dot), 3));
	}
	else
	{
		memcpy(&fcb->F[0], filename, MIN(strlen(filename), 8));
	}
	for (i = 0; i < 11; i++)
	{
		fcb->F[i] = toupper(fcb->F[i]);
		if (fcb->F[i] == '*' || fcb->F[i] == '?' || fcb->F[i] == '\\' || fcb->F[i] == '/')
			return FALSE;
	}
	
	return TRUE;
}

