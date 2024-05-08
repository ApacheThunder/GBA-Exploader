	.TEXT
	.ARM
	.ALIGN

@---------------------------------------------------------------------------------------
.GLOBAL     LinkReset_ARM9
.func LinkReset_ARM9
@---------------------------------------------------------------------------------------
LinkReset_ARM9:
	MOV     R12, #0x4000000
	LDR     R1, [R12,#0x130]
@	ANDS    R1,R1,#0x30C		@L+R+START+SELECT
@	BXNE	LR

	MOV     R10, #0
	MCR     p15, 0, R10,c7,c5,0
	MCR     p15, 0, R10,c7,c6,0
	MCR     p15, 0, R10,c7,c10, 4

	MCR     p15, 0, R10,c3,c0,0
	MCR     p15, 0, R10,c1,c0

	LDR	R6,=0x40001A4
	MOV	R2,#0xC0
	STRB	R2,[R6,#-3]

	LDR	R2,=0xededdede
        STR     R2, [R6,#4]
	MOV	R0,#0x2
	MOV	R0,R0,LSL #24
        STR     R0, [R6,#8]

	MOV	R0,#0xA0000000
	STR	R0,[R6]
Wait_Busy00:
	LDR	R0,[R6]
	TST	R0,#0x80000000
	BNE	Wait_Busy00

	ADD	R7,R12,#0x200
	ADR	R0,reset+1
	BX	R0

	.LTORG
	.ALIGN


	.THUMB
	.ALIGN
@---------------------------------------------------------------------------------------
reset:
	MOV	R1,#0x21
	LSL	R1,R1,#20		@LDR	R1,=0x2100000

	STRH    R1, [R7,#0x8]		@[4000208]=0
	STRH	R1, [R7,#0x4]

	MOV	R4,#0x5C
	SUB	R6,R7,R4

	LDR	R0,=0x100200A4
	STR	R0,[R6,#4]
	STRB	R1,[R6,#8]

	MOV	R4,#0xAA
	LSL	R4,R4,#24
	STR	R4,[R6]

	MOV	R5,#0x40
	LSL	R3,R5,#17
	LSL	R5,R5,#4

	MOV	R4,#0x41
	LSL	R4,R4,#20

Wait_Ready:
	LDR	R0,[R6]
	TST	R0,R3
	BEQ	Wait_Ready

	LDR	R0,[R4,#0x10]
	STR	R0,[R1]
	ADD	R1,R1,#4

	SUB	R5,R5,#4
	BNE	Wait_Ready

	LDR	R0,=0xE880
	STRH	R0,[R7,#0x4]		@WAIT_CR=0xE880

	LDR     R0, =0xE59FF018
	LDR     R2, =0x27FFE04
	STR     R0, [R2]
	STR     R2, [R2,#0x20]

	BX	R2

.endfunc

	.end
