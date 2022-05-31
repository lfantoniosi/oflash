#include "types.h"
#include "stdio.h"
#include "flash.h"

uchar VendorID = 0;
uchar DeviceID = 0;
uchar FlashModel = FLASH_NONE;

bool DetectFlashROM()
{
	// try to detect the MX29
	WrSlt(SLOT_ID(0, 0), 0x0555, 0xaa); // just making sure the flashrom is visible in all these slots
	WrSlt(SLOT_ID(3, 0), 0x02aa, 0x55);
	WrSlt(SLOT_ID(3, 1), 0x0555, 0x90);

	VendorID = RdSlt(SLOT_ID(3, 3), 0x0000);
	DeviceID = RdSlt(SLOT_ID(0, 0), 0x0001);

	WrSlt(SLOT_ID(0, 0), 0x0555, 0xf0);
	
	if (VendorID == 0xc2 && DeviceID == 0xa4)
	{
		printf("FlashROM MX29F040 detected.\r\n");
		FlashModel = FLASH_MX29;
		return TRUE;
	}

	// if not try to detect the SST39
	WrSlt(SLOT_ID(0, 0), 0x5555, 0xaa);
	WrSlt(SLOT_ID(3, 0), 0x2aaa, 0x55);
	WrSlt(SLOT_ID(3, 1), 0x5555, 0x90);
	
	VendorID = RdSlt(SLOT_ID(3, 3), 0x0000);
	DeviceID = RdSlt(SLOT_ID(0, 0), 0x0001);
	
	WrSlt(SLOT_ID(0, 0), 0x5555, 0xf0);
	
	if (VendorID == 0xbf && DeviceID == 0xb7)
	{
		printf("FlashRom SST39SF040 detected.\r\n");
		FlashModel = FLASH_SST39;
		return TRUE;
	}
	return FALSE;
}

uchar WaitToggleBit(uchar slotId, uint address, uchar bitMask)
{
    uchar a, b;

    do{
        a = RdSlt(slotId, address) & bitMask;
        b = RdSlt(slotId, address) & bitMask;

        if(a == b)
		{
            a = RdSlt(slotId, address) & bitMask;
            b = RdSlt(slotId, address) & bitMask;
        }

    } while(a != b);

	return a;
}

void EraseBank(uchar slotId)
{
	uint i;
	if (FlashModel == FLASH_MX29)
	{
		WrSlt(SLOT_ID(0, 0), 0x0555, 0xaa);
		WrSlt(SLOT_ID(0, 0), 0x02aa, 0x55);
		WrSlt(SLOT_ID(0, 0), 0x0555, 0x80);
		WrSlt(SLOT_ID(0, 0), 0x0555, 0xaa);
		WrSlt(SLOT_ID(0, 0), 0x02aa, 0x55);
		WrSlt(slotId, 0x0000, 0x30);
		WaitToggleBit(slotId, 0x0000, (uchar)(1 << 6));
		for(;WaitToggleBit(slotId, 0x0000, (uchar)(1 << 3)) != (uchar)(1<<3););
		WaitToggleBit(slotId, 0x0000, (uchar)(1 << 6));
		for(;WaitToggleBit(slotId, 0x0000, (uchar)(1 << 7)) != (uchar)(1<<7););
		WaitToggleBit(slotId, 0x0000, (uchar)(1 << 7));
		WrSlt(SLOT_ID(0, 0), 0x0555, 0xf0);
	} 
	else
	{
		for(i = 0; i < 8; i ++)
		{
			WrSlt(SLOT_ID(0, 0), 0x5555, 0xaa);
			WrSlt(SLOT_ID(0, 0), 0x2aaa, 0x55);
			WrSlt(SLOT_ID(0, 0), 0x5555, 0x80);
			WrSlt(SLOT_ID(0, 0), 0x5555, 0xaa);
			WrSlt(SLOT_ID(0, 0), 0x2aaa, 0x55);
			WrSlt(slotId, i, 0x30);
			WaitToggleBit(slotId, (i << 12), (uchar)(1 << 6));
			WrSlt(SLOT_ID(0, 0), 0x5555, 0xf0);
		}
	}
}

void EraseSectors()
{
	// omega flash rom is split across multiple slots
	printf("\r\nErasing 64K banks");
	EraseBank(SLOT_ID(0, 0));
	printf(".");
	EraseBank(SLOT_ID(3, 0));
	printf(".");
	EraseBank(SLOT_ID(3, 1));
	printf(".");
	EraseBank(SLOT_ID(3, 3));
	printf(".");
	printf("done.\r\n");
}


///

void WrSlt_0200(uchar slotId, uint address, uchar byte);
void WrSlt_8000(uchar slotId, uint address, uchar byte);

void WrSlt(uchar slotId, uint address, uchar byte)
{
	if (address < 0x8000)
	{
		WrSlt_8000(slotId, address, byte);
	}
	else
	{
		WrSlt_0200(slotId, address, byte);
	}
}

uchar RdSlt_0200(uchar slotId, uint address);
uchar RdSlt_8000(uchar slotId, uint address);

uchar RdSlt(uchar slotId, uint address)
{
	if (address < 0x8000)
	{
		return RdSlt_8000(slotId, address);
	}
	else
	{
		return RdSlt_0200(slotId, address);
	}
}

uchar PrSlt_0200(uchar slotId, uint address, uchar byte);
uchar PrSlt_8000(uchar slotId, uint address, uchar byte);

uchar PrSlt(uchar slotId, uint address, uchar byte)
{
	if (address < 0x8000)
	{
		return PrSlt_8000(slotId, address, byte);
	}
	else
	{
		return PrSlt_0200(slotId, address, byte);
	}
}

void Area_0200() __naked
{
	__asm
	.area		_HEADER (ABS)
	.org 		0x0200
	.ds	64
_localStack_0200::
_stack_0200::
	.ds 2
_slotId_0200::
	.ds 1
_address_0200::
	.ds 2
_byte_0200::
	.ds 1
_flashModel_0200:
	.ds 1
	__endasm;
}

extern uchar slotId_0200;
extern volatile uchar* address_0200;
extern uchar byte_0200;
extern uchar flashModel_0200;

void WrSlt_0200_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x0f) | ((slotId_0200 & 0x03) << 6) | ((slotId_0200 & 0x03) << 4); // sets pages 3 and 2

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_0200 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x0f) | ((slotId_0200 & 0x0c) << 4) | ((slotId_0200 & 0x0c) << 2); // set subslots for pages 3 and 2
	}
	
	*address_0200 = byte_0200;

	if (slotId_0200 & 0x80)
	{
		SLTREG = SltReg; // restore subslots
	}
	
	PPIA = PPIReg; // restore slots
}

void WrSlt_0200(uchar slotId, uint address, uchar byte)
{
	slotId_0200 = slotId;
	address_0200 = (char *) address;
	byte_0200 = byte;

	// use local stack close to the code
	__asm
		ld		(_stack_0200), sp
		ld		sp, #_localStack_0200
	__endasm;

	WrSlt_0200_local();

	__asm
		ld		sp,(_stack_0200)
	__endasm;
}

void RdSlt_0200_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x0f) | ((slotId_0200 & 0x03) << 6) | ((slotId_0200 & 0x03) << 4); // sets pages 3 and 2

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_0200 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x0f) | ((slotId_0200 & 0x0c) << 4) | ((slotId_0200 & 0x0c) << 2); // set subslots for pages 3 and 2
	}
	
	byte_0200 = *address_0200;

	if (slotId_0200 & 0x80)
	{
		SLTREG = SltReg; // restore subslots
	}
	
	PPIA = PPIReg; // restore slots
}

uchar RdSlt_0200(uchar slotId, uint address)
{
	slotId_0200 = slotId;
	address_0200 = (char *) address;

	// use local stack close to the code
	__asm
		ld		(_stack_0200), sp
		ld		sp, #_localStack_0200
	__endasm;

	RdSlt_0200_local();

	__asm
		ld		sp,(_stack_0200)
	__endasm;

	return byte_0200;
}

void PrSlt_0200_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
    uchar a,b;
    volatile uchar* x555;
    volatile uchar* x2aa;

	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x0f) | ((slotId_0200 & 0x03) << 6) | ((slotId_0200 & 0x03) << 4); // sets pages 3 and 2

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_0200 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x0f) | ((slotId_0200 & 0x0c) << 4) | ((slotId_0200 & 0x0c) << 2); // set subslots for pages 3 and 2
	}

	if (flashModel_0200 == FLASH_MX29)
	{
		x555 = (uchar*)0x8555;
		x2aa = (uchar*)0x82aa;
	} 
	else
	{
		x555 = (uchar*)0xD555;
		x2aa = (uchar*)0xaaaa;
	}

    *x555 = 0xaa;
    *x2aa = 0x55;
    *x555 = 0xa0;
 
	*address_0200 = byte_0200;

    do{
        a = *address_0200  & (1 << 6);
        b = *address_0200  & (1 << 6);

        if(a == b)
		{
            a = *address_0200  & (1 << 6);
            b = *address_0200  & (1 << 6);
        }

    } while(a != b);

    if (byte_0200 == *address_0200)
        byte_0200 = TRUE;
    else
        byte_0200 = FALSE;

	if (slotId_0200 & 0x80)
	{
		SLTREG = SltReg; // restore subslots 
	}
	
	PPIA = PPIReg; // restore slots 
}

uchar PrSlt_0200(uchar slotId, uint address, uchar byte)
{
	slotId_0200 = slotId;
	address_0200 = (char *) address;
	byte_0200 = byte;
	flashModel_0200 = FlashModel;

	// use local stack close to the code
	__asm
		ld		(_stack_0200), sp
		ld		sp, #_localStack_0200
	__endasm;

	PrSlt_0200_local();

	__asm
		ld		sp,(_stack_0200)
	__endasm;

    return byte_0200;
}

////

void Area_8000() __naked
{
	__asm
	.area		_HEADER (ABS)
	.org 		0x8000
	.ds	64
_localStack_8000::
_stack_8000::
	.ds 2
_slotId_8000::
	.ds 1
_address_8000::
	.ds 2
_byte_8000::
	.ds 1
_flashModel_8000::
	.ds 1
	__endasm;
}

extern uchar slotId_8000;
extern volatile uchar* address_8000;
extern uchar byte_8000;
extern uchar flashModel_8000;

void WrSlt_8000_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x30) | ((slotId_8000 & 0x03) << 6) | ((slotId_8000 & 0x03) << 2) | (slotId_8000 & 0x03); // sets pages 3, 1 and 0

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_8000 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x30) | ((slotId_8000 & 0x0c) << 4) | ((slotId_8000 & 0x0c)) | ((slotId_8000 & 0x0c) >> 2); // set subslots for pages 3, 1 and 0
	}
	
	*address_8000 = byte_8000;

	if (slotId_8000 & 0x80)
	{
		SLTREG = SltReg; // restore subslots 
	}
	
	PPIA = PPIReg; // restore slots 
}

void WrSlt_8000(uchar slotId, uint address, uchar byte)
{
	slotId_8000 = slotId;
	address_8000 = (char *) address;
	byte_8000 = byte;

	// use local stack close to the code
	__asm
		ld		(_stack_8000), sp
		ld		sp, #_localStack_8000
	__endasm;

	WrSlt_8000_local();

	__asm
		ld		sp,(_stack_8000)
	__endasm;
}

void RdSlt_8000_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x30) | ((slotId_8000 & 0x03) << 6) | ((slotId_8000 & 0x03) << 2) | (slotId_8000 & 0x03); // sets pages 3, 1 and 0

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_8000 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x30) | ((slotId_8000 & 0x0c) << 4) | ((slotId_8000 & 0x0c)) | ((slotId_8000 & 0x0c) >> 2); // set subslots for pages 3, 1 and 0
	}
	
	byte_8000 = *address_8000;

	if (slotId_8000 & 0x80)
	{
		SLTREG = SltReg; // restore subslots 
	}
	
	PPIA = PPIReg; // restore slots 
}

uchar RdSlt_8000(uchar slotId, uint address)
{
	slotId_8000 = slotId;
	address_8000 = (char *) address;

	// use local stack close to the code
	__asm
		ld		(_stack_8000), sp
		ld		sp, #_localStack_8000
	__endasm;

	RdSlt_8000_local();

	__asm
		ld		sp,(_stack_8000)
	__endasm;

	return 	byte_8000;
}

void PrSlt_8000_local()
{
	// disable interrupts (keep it disabled for the rest of program)
	uchar PPIReg;
	uchar SltReg;
    uchar a,b;
    volatile uchar* x555;
    volatile uchar* x2aa;

	__asm
		di 
	__endasm;

	PPIReg = PPIA; // save current slot config
	PPIA = (PPIReg & 0x30) | ((slotId_8000 & 0x03) << 6) | ((slotId_8000 & 0x03) << 2) | (slotId_8000 & 0x03); // sets pages 3, 1 and 0

	SltReg = ~SLTREG; // save current subslot config
	if (slotId_8000 & 0x80) // is a expanded slot ?
	{
		SLTREG = (SltReg & 0x30) | ((slotId_8000 & 0x0c) << 4) | ((slotId_8000 & 0x0c)) | ((slotId_8000 & 0x0c) >> 2); // set subslots for pages 3, 1 and 0
	}

	if (flashModel_8000 == FLASH_MX29)
	{
		x555 = (uchar*)0x0555;
		x2aa = (uchar*)0x02aa;
	} 
	else
	{
		x555 = (uchar*)0x5555;
		x2aa = (uchar*)0x2aaa;
	}

	*x555 = 0xaa;
	*x2aa = 0x55;
	*x555 = 0xa0;
	
    *address_8000 = byte_8000;

    do {
        a = *address_8000  & (1 << 6);
        b = *address_8000  & (1 << 6);

        if(a == b)
		{
            a = *address_8000  & (1 << 6);
            b = *address_8000  & (1 << 6);
        }

    } while(a != b);

    if (byte_8000 == *address_8000)
        byte_8000 = TRUE;
    else
        byte_8000 = FALSE;

	if (slotId_8000 & 0x80)
	{
		SLTREG = SltReg; // restore subslots 
	}
	
	PPIA = PPIReg; // restore slots 
}

uchar PrSlt_8000(uchar slotId, uint address, uchar byte)
{
	slotId_8000 = slotId;
	address_8000 = (char *) address;
	byte_8000 = byte;
	flashModel_8000 = FlashModel;

	// use local stack close to the code
	__asm
		ld		(_stack_8000), sp
		ld		sp, #_localStack_8000
	__endasm;

	PrSlt_8000_local();

	__asm
		ld		sp,(_stack_8000)
	__endasm;

    return byte_8000;
}