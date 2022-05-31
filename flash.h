#ifndef _FLASH_H
#define _FLASH_H_

#include "types.h"

#define FLASH_SST39 0
#define FLASH_MX29  1
#define FLASH_NONE	255

extern uchar VendorID;
extern uchar DeviceID;
extern uchar FlashModel;

#define SLOT_ID(SLOT, SSLOT) (EXPTBL[SLOT] | (SSLOT << 2) | SLOT)

void WrSlt(uchar slotId, uint address, uchar byte);
uchar RdSlt(uchar slotId, uint address);
uchar PrSlt(uchar slotId, uint address, uchar byte);
uchar DetectFlashROM();
void EraseSectors();
void Program();

#endif