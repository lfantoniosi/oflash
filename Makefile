CC = sdcc
CFLAGS = -mz80 --opt-code-size --std-sdcc11
ASM = sdasz80
ASMFLAGS =

%.asm : %.c
	$(CC) $(CFLAGS) -S $<

oflash.com: crt0.rel oflash.rel bdos.rel stdio.rel flash.rel
	$(CC) $(CFLAGS) --data-loc 0 --code-loc 0x1000 --no-std-crt0 $^ -ooflash.ihx
	hex2bin -e com oflash.ihx
	cp -f oflash.com disk/oflash.com

all: of.com

oflash.rel: oflash.c types.h bdos.h stdio.h

bdos.rel: bdos.c types.h bdos.h

stdio.rel: stdio.c types.h stdio.h 

flash.rel: flash.c types.h stdio.h


%.rel : %.c
	$(CC) $(CFLAGS) -o$@ -c $<

%.rel : %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	+rm *.rel *.asm *.lst *.sym *.map *.ihx oflash.com *.lk *.noi
	

