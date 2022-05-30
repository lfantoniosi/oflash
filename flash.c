#include "types.h"
#include "stdio.h"

__at(0x0555) uchar MX29_0555;
__at(0x02aa) uchar MX29_02aa;

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
	__endasm;
}

extern uchar slotId_0200;
extern volatile uchar* address_0200;
extern uchar byte_0200;

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
    volatile uchar* mx29_x555;
    volatile uchar* mx29_x2aa;

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

    mx29_x555 = (uchar*)(((uint)address_0200 & 0xC000) | 0x0555);
    mx29_x2aa = (uchar*)(((uint)address_0200 & 0xC000) | 0x02aa);

    *mx29_x555 = 0xaa;
    *mx29_x2aa = 0x55;
    *mx29_x555 = 0xa0;
 
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
	__endasm;
}

extern uchar slotId_8000;
extern volatile uchar* address_8000;
extern uchar byte_8000;

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
    volatile uchar* mx29_x555;
    volatile uchar* mx29_x2aa;

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
    mx29_x555 = (uchar*)(((uint)address_8000 & 0xC000) | 0x0555);
    mx29_x2aa = (uchar*)(((uint)address_8000 & 0xC000) | 0x02aa);

    *mx29_x555 = 0xaa;
    *mx29_x2aa = 0x55;
    *mx29_x555 = 0xa0;
	
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