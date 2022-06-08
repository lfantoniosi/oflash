#include<stdio.h>
#include<string.h>

#include "types.h"
#include "bdos.h"
#include "stdio.h"
#include "flash.h"

bool Proceed = TRUE;
uchar DMABuf[128];
void Program();

int main(int c, char** argv)
{
	c; argv;
	uchar value;
	int n, xy;

	iniTxt();
	printf("Maple as Honey: Omega MSX FlashROM Programmer v0.4\r\n");
	printf("2022 by retrocanada\r\n\r\n");

	if (EXPTBL[0] != 0 || EXPTBL[3] == 0)
	{
		// slot 0 should not be expanded
		// slot 3 should be expanded
		printf("Error: Slot configuration is wrong for an Omega MSX.\r\nIf you have changed the default SPLD slot configuration you need to change this tool.\r\n");
		Proceed = FALSE;
	}

	if (Proceed)
	{
		// check for 512KB at least
		for(n = 4; n < 20; n++)
		{
			MM_8000 = (uchar)n;
			value = ~RdSlt(SLOT_ID(3,2), 0x8000);
			WrSlt(SLOT_ID(3,2), 0x8000, value);
			if (value != RdSlt(SLOT_ID(3,2), 0x8000))
			{
				// not ram
				MM_8000 = 1;
				Proceed = FALSE;
				printf("Error: RAM is not in slot 3.2.\r\nIf you have changed the default SPLD slot configuration you need to change this tool.\r\n");
				break;
			}

			WrSlt(SLOT_ID(3,2), 0x8000, 0); // zeroe them
		}	

		MM_8000 = 1;

		if (Proceed)
		{
			for(n = 4; n < 20; n++)
			{
				MM_8000 = (uchar)n;
				if (RdSlt(SLOT_ID(3,2), 0x8000) != 0)
				{
					// wrap-up, not enough ram
					MM_8000 = 1;
					Proceed = FALSE;
					printf("Error: at least 512K are required in slot 3.2\r\n");
					break;
				}
				WrSlt(SLOT_ID(3,2), 0x8000, n);
			}
		}

		MM_8000 = 1;

		if (Proceed)
		{
			Proceed = DetectFlashROM();

			if (!Proceed)
			{
				printf("\r\nERROR: FlashROM not detected...\r\n");
			}
			else
			{
				ENABLE_INT;
				if (bdos_f_open(&MAIN_FCB) == BDOS_OK)
				{
					bdos_f_dmaoff(DMABuf); 
					printf("Loading file:");
					xy = getXY();
					for(int n = 0; n < 2048; n++)
					{
						if (bdos_f_read(&MAIN_FCB) != BDOS_OK)
						{
							printf("\r\nError reading the ROM file\r\n");
							Proceed = FALSE;
							break;
						}
						DISABLE_INT;
						MM_8000 = 4 + (n >> 7);
						memcpy((void*)(0x8000 + ((n & 0x007f) << 7)), DMABuf, 128);
						MM_8000 = 1;
						ENABLE_INT;
						if (((n+1) & 0x007f) == 0)
						{
							setXY(xy);
							printf("% 4dKB", ((n+1) >> 3));
						} 
					}

					bdos_f_close(&MAIN_FCB);

					if (Proceed)
					{
						DISABLE_INT;

						printf(" done.\r\n\r\nThe system is running entirely from RAM now.\r\nIt is safe to switch the J1 jumper for the desired 256KB bank.\r\nAs a good practice always program the opposite bank you booted this system.\r\n\r\nPress any key to start programming the flash ROM.\r\n");
						while(is_key_pressed());
						while(!is_key_pressed());

						EraseSectors();
						Program();
					}
				}
				else
				{
					printf("\r\nError opening the ROM file\r\n");
					Proceed = FALSE;
				}
			}
		}
	}
	

	printf("\r\nPress any key to continue...\r\n");
	while(!is_key_pressed());

	iniTxt();

	return 0;
}


void Program()
{
	uint p, addr, xy = 0;
	uchar *ramAddr;
	ulong romAddr;
	uchar byte;
	printf("\r\nProgramming Flash ROM\r\n");
	romAddr = 0x0000;
	for(p = 0; p < 16; p++)
	{
		romAddr = ((ulong)p) << 14;

		if ((romAddr & 0xffff) == 0)
		{
			printf("\r\nBank %d:", (uint)(romAddr >> 16));
			xy = getXY();
		}

		for(ramAddr = (uchar*)0x8000; ramAddr < (uchar*)0xC000;)
		{
			MM_8000 = p + 4;
			byte = *ramAddr;
			MM_8000 = 1;

			addr = (uint)(romAddr & 0xFFFF);

			if (romAddr < 0x10000)
			{
				if (PrSlt(SLOT_ID(0,0), addr, byte))
				{
					romAddr++;
					ramAddr++;
				}
			} 
			else if (romAddr < 0x20000)
			{
				// cant' write the slot register on expanded slot
				if (addr != 0xffff)
				{
					if (PrSlt(SLOT_ID(3,0), addr, byte))
					{
						romAddr++;
						ramAddr++;
					}
				}
				else
				{
					romAddr++;
					ramAddr++;
				}
			}
			else if (romAddr < 0x30000)
			{	
				// cant' write the slot register on expanded slot
				if (addr != 0xffff)
				{
					if (PrSlt(SLOT_ID(3,1), addr, byte))
					{
						romAddr++;
						ramAddr++;
					}
				}
				else
				{
					romAddr++;
					ramAddr++;
				}
			}
			else
			{	
				// cant' write the slot register on expanded slot
				if (addr != 0xffff)
				{
				
					if (PrSlt(SLOT_ID(3,3), addr, byte))
					{
						romAddr++;
						ramAddr++;
					}
				}
				else
				{
					romAddr++;
					ramAddr++;
				}
			}

			if (((uint)(ramAddr+1) & 0x03ff) == 0)
			{
				setXY(xy);
				printf("% 4dKB", ((romAddr & 0xffff) + 1) >> 10);
			}
		}
	}
	WrSlt(SLOT_ID(0, 0), 0x0555, 0xf0);
	printf("\r\n done.\r\n\r\nPLEASE RESET THE SYSTEM!");

	__asm
		halt
	__endasm;
}
