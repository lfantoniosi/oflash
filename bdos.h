#ifndef _BDOS_H_
#define _BDOS_H_

#include "types.h"

typedef struct fcb_t 
{
	uchar DR;		// drive number
	char  F[8];		// file name
	char  T[3];		// extension type
	uchar EX;		// current block
	uchar S1;		// number of blocks from the top
	uchar S2;		// record size
	uchar RC;		// record size
	ulong FSIZE;	// filesize
	uchar DATE[2];  // date last acess
	uchar TIME[2];  // time last acess
	uchar AL[8];	// platform independent
	uchar CR;		// current record
	uchar R[3];		// random record number (16-bit R[0]-R[1] with R[2] overflow)
} fcb_t;

uchar bdos_drv_get();

bool bdos_init_fcb(fcb_t* fcb, const char* filename);
void bdos_f_dmaoff(void* dmaoff);
uint bdos_f_open(fcb_t* fcb);
uint bdos_f_close(fcb_t* fcb);
uint bdos_f_make(fcb_t* fcb);
uchar bdos_drv_get();
uint bdos_drv_set(uchar drive);
uint bdos_f_read(fcb_t* fcb);
uint bdos_f_write(fcb_t* fcb);
uint bdos_f_delete(fcb_t* fcb);

uint bdos_f_rnd_readout(fcb_t* fcb, uint count);

#define BDOS 0x0005

#define BDOS_DRV_SET 	14
#define BDOS_F_OPEN 	15
#define BDOS_F_CLOSE	16
#define BDOS_F_SFIRST	17
#define BDOS_F_NEXT		18
#define BDOS_F_DELETE	19
#define BDOS_F_READ     20
#define BDOS_F_WRITE    21
#define BDOS_F_MAKE     22
#define BDOS_F_RENAME   23
#define BDOS_DRV_GET	25
#define BDOS_F_DMAOFF	26
#define BDOS_F_READOUT  39

#define BDOS_OK						0
#define BDOS_EOF					1
#define BDOS_DIR_FULL				1
#define BDOS_DISC_FULL				2
#define BDOS_INVALID_FCB			9
#define BDOS_MEDIA_CHANGED			10
#define BDOS_HW_ERROR				255

#define HW_FILE_NOT_FOUND			0
#define HW_DRIVE_SELECT				1
#define HW_READ_ONLY_DISK			2
#define HW_READ_ONLY_FILE			3
#define HW_INVALID_DRIVE			4
#define HW_FILE_ALREADY_OPEN		5
#define HW_FILE_ALREADY_EXISTS		8
#define HW_FILENAME_ERROR			9

__at(0x005c) fcb_t MAIN_FCB;

#endif
