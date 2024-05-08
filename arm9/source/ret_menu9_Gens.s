@***********************************************************
@
@	by Rudolph (çcíÈ)	2007/10/22
@
@  SoftwareReset Routines for General purpose.
@************************************************************

 .ALIGN
 .GLOBAL	ret_menu9_GENs
 .CODE 32
 .ARM

ret_menu9_GENs:
	mov	r4, #0x4000000
	str	r4, [r4, #+0x208]		@ *(04000208)=#0x4000000
	mov	r7, #0xE000
	str	r7, [r4, #+0x204]		@ *(04000204)=#0xE000

	mov	r4, #0x4000000
	mov	r0, #0x0
	mvn	r1, #0x0
	str	r0, [r4, #+0x208]		@ *(04000208)=#0
	str	r0, [r4, #+0x210]		@ *(04000210)=#0
	str	r1, [r4, #+0x214]		@ *(04000214)=#0

	mov	r1, #0x0
mcr_lp0:
	mov	r0, #0x0
mcr_lp1:
	orr	r2, r1, r0
	mcr	15, 0, r2, cr7, cr14, 2
	add	r0, r0, #0x20
	cmp	r0, #0x400
	bne	mcr_lp1

	add	r1, r1, #0x40000000
	cmp	r1,#0x0
	bne	mcr_lp0

	mov	r10, #0x0
	mcr	15, 0, r10, cr7, cr5, 0
	mcr	15, 0, r10, cr7, cr6, 0
	mcr	15, 0, r10, cr7, cr10, 4
	mcr	15, 0, r10, cr3, cr0, 0
	mcr	15, 0, r10, cr2, cr0, 0
	mov	r10, #0x800000
	add	r10, r10, #0xA
	mcr	15, 0, r10, cr9, cr1, 0
	mov	r10, #0xC
	mcr	15, 0, r10, cr9, cr1, 1
	mov	r10, #0x1F
	msr	CPSR_fc, r10
	ldr	r2, =0x00803000
	mov	r3, #0x0
	str	r3, [r2, #+0xFFC]		@ *(00803ffc)=#0
	mvn	r3, #0x0
	str	r3, [r2, #+0xFF8]		@ *(00803ff8)=#0xFFFFFFFF

	mov	r1, #0x0
	ldr	r2, =0x040000B0
	ldr	r4, =0x04000100
	mov	r0,r1

TimDMA_lp:
	str	r1, [r2], #+0x4
	str	r1, [r2], #+0x4
	str	r1, [r2], #+0x4
	strh	r1, [r4], #+0x2
	strh	r1, [r4], #+0x2
	add	r0, r0, #0x1
	cmp	r0, #0x4
	bne	TimDMA_lp

	mov	r0, #0x0
	ldr	r4, =0x027FFE04
	str	r0, [r4, #+0x0]			@ *(027ffe04)=#0
	ldr	r3, =0x80808080
	mov	r1, #0x4000000
	str	r3, [r1, #+0x240]		@ *(04000240)=#0x80808080
	mov	r3, #0x5000000
	mvn	r2, #0x0
	strh	r2, [r3, #+0x0]			@ *(05000000)=#0xFFFF
	add	r3, r3, #0x2
	str	r4, [r1, #+0xD4]		@ *(040000d4)=#0x027ffe04
	ldr	r2, =0x850001FF
	str	r3, [r1, #+0xD8]		@ *(040000d8)=#0x5000002
	str	r2, [r1, #+0xDC]		@ *(040000dc)=#0x850001ff

DMA_lp0:
	ldr	r3, [r1, #+0xDC]		@ r3 = *(040000dc)
	cmp	r3, #0x0
	blt	DMA_lp0

	mov	r2, #0x7000000
	str	r4, [r1, #+0xD4]		@ *(040000d4)=#0x027ffe04
	ldr	r3, =0x85000200
	str	r2, [r1, #+0xD8]		@ *(040000d8)=#0x7000000
	str	r3, [r1, #+0xDC]		@ *(040000dc)=#0x85000200
	add	r2, r2, #0xFD000000

DMA_lp1:
	ldr	r3, [r2, #+0xdc]
	cmp	r3, #0x0
	blt	DMA_lp1

	mov	r1, #0x4000000
	str	r4, [r2, #+0xD4]
	ldr	r3, =0x85000015
	str	r2, [r2, #+0xD8]
	str	r3, [r2, #+0xDC]
DMA_lp2:
	ldr	r3, [r1, #+0xDC]
	cmp	r3, #0x0
	blt	DMA_lp2

	ldr	r2, =0x04001000
	str	r4, [r1, #+0xD4]
	ldr	r3, =0x85000015
	str	r2, [r1, #+0xD8]
	str	r3, [r1, #+0xDC]
	mov	r1, #0x4000000
DMA_lp3:
	ldr	r3,[r1, #+0xDC]
	cmp	r3,#0x0
	blt	DMA_lp3


	mov	r2, #0x6800000
	str	r4, [r1, #+0xD4]
	ldr	r3, =0x85029000
	str	r2, [r1, #+0xD8]
	mov	r12, #0x4000000
	str	r3, [r1, #+0xDC]
DMA_lp4:
	ldr	r3, [r12, #+0xDC]
	cmp	r3, #0x0
	blt	DMA_lp4

	ldr	r1, =0x04000200
	ldr	r2, =0x04001000
	mov	r3, #0x0
	strh	r3, [r12, #+0x4]	@ *(04000004)=#0
	ldr	r0, =0xffff820f
	str	r3, [r12, #+0x0]	@ *(04000000)=#0
	str	r3, [r2, #+0x0]		@ *(04001000)=#0
	strb	r3, [r1, #+0x40]	@ *(04000240)=#0
	mov	r2, #0x3000000
	strb	r3, [r1, #+0x41]	@ *(04000241)=#0
	strb	r3, [r1, #+0x42]	@ *(04000242)=#0
	strb	r3, [r1, #+0x43]	@ *(04000243)=#0
	strb	r3, [r1, #+0x44]	@ *(04000244)=#0
	strb	r3, [r1, #+0x45]	@ *(04000245)=#0
	strb	r3, [r1, #+0x46]	@ *(04000246)=#0
	strb	r3, [r1, #+0x48]	@ *(04000248)=#0
	strb	r3, [r1, #+0x49]	@ *(04000249)=#0
	ldr	r3, =0x04000300
	str	r2, [r12, #+0x240]	@ *(04000240)=#0x3000000
	mov 	r2, #0x3
	strh	r0, [r3, #+0x4]		@ *(04000304)=0xffff820f
	strb	r2, [r1, #+0x47]	@ *(04000247)=#0x3


	mov	r3, #0x4000000
	ldr	r5, =0x0000e880
	str	r5, [r3, #+0x204]	@ *(04000204)=#0x0000e880


     	ldr	r0, =0x027FFDF8
     	ldr	r1, =0xE51FF004
     	str	r1, [r0, #0x0]			@ (027ffdf8)=E51FF004:ldr r15,[r15, #-0x4]
     	str	r0, [r0, #0x4]			@ (027ffdfC)=027FFDF8

	bx	r0				@ JUMP 027FFDF8

 .END

