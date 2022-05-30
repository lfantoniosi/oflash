	.module 	crt0	
    .globl		_main
	.globl 		l__DATA
	.globl 		s__DATA
	.globl 		l__INITIALIZER
	.globl 		s__INITIALIZED
	.globl 		s__INITIALIZER
	
	.area		_HEADER (ABS)
	
	.org		0x0100
init:: 
	call    	gsinit
	
	ld		de,#argv
	ld		hl,#0x0080
	ld		a,(hl)
	or		a
	jr		z,param_count
	inc		hl
first_param:
	ld		a,(hl)
	cp		#0x0d
	jr		z,last_param
	cp		#0x20
	jr		nz,valid_param
	inc		hl
	jr		first_param
valid_param:
	ex		de,hl
	ld		(hl),e
	inc		hl
	ld		(hl),d
	inc		hl
	ex		de,hl
next_param:
	inc		hl
	ld		a,(hl)
	cp		#0x0d
	jr		z,last_param
	cp		#0x20
	jr		nz,next_param
	xor		a
	ld		(hl),a
	inc		hl
	jr		first_param
last_param:
	xor		a
	ld		(hl),a
	
param_count:
	ex		de,hl
	ld		de,#argv
	or		a
	sbc		hl,de
	srl		h
	rr		l

	push		de
	push		hl
	
	call		_main

	pop		af
	pop		af
_exit::
	ld		c,#0
	call		5

argv:	; hold up to 64 parameters
	.ds		128
	.area	_SLOT
	.area	_HOME
	.area	_CODE	
	.area	_INITIALIZER
	.area   _GSINIT
	.area   _GSFINAL

	.area	_DATA
	.area	_INITIALIZED
	.area	_BSEG
	.area   _BSS
	.area	_HEAP	
	.area   _GSINIT
gsinit::
	; Default-initialized global variables.
	ld      bc, #l__DATA
	ld      a, b
	or      a, c
	jr      Z, zeroed_data
	ld      hl, #s__DATA
	ld      (hl), #0x00
	dec     bc
	ld      a, b
	or      a, c
	jr      Z, zeroed_data
	ld      e, l
	ld      d, h
	inc     de
	ldir
zeroed_data:
	; Explicitly initialized global variables.
	ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jr		Z, gsinit_next
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir

gsinit_next:

	.area   _GSFINAL
	ret
