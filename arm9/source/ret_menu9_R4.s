@***********************************************************
@
@	by Rudolph (çcíÈ)
@		2007/05/30 Initialization bug correction
@		2007/05/24 First release
@
@-----------------------------------------------------------
@  SoftwareReset Routines for R4DS or M3SimplyDS.
@
@ Redistribution and use in source and binary forms,
@ with or without modification, are permitted provided 
@ that the following conditions are met:
@
@  Only the Homebrew application can be used. 
@  It is not possible to use it by a business purpose. 
@
@  This software is made based on information obtained 
@  by Reverse engineering. 
@
@  Please use that at once when a source code that is 
@  more formal than the official is open to the public.
@************************************************************

 .ALIGN
 .GLOBAL	ret_menu9_R4
 .CODE 32
 .ARM

ret_menu9_R4:

	mov	r0, #0x2000
	orr	r0, r0, #0x78
	mov	r1, #0x00
	mcr	15, 0, r0, cr1, cr0, 0
	mcr	15, 0, r1, cr7, cr5, 0
	mcr	15, 0, r1, cr7, cr6, 0
	mcr	15, 0, r1, cr7, cr10, 4
	orr	r0, r0, #0x50000
	mcr	15, 0, r0, cr1, cr0, 0

     	ldr	r0, =0x027FFDF8
     	ldr	r1, =0xE51FF004
     	str	r1, [r0, #0x0]			@ (027ffdf8)=E51FF004:ldr r15,[r15, #-0x4]
     	str	r0, [r0, #0x4]			@ (027ffdfC)=027FFDF8

	bx	r0				@ JUMP 027FFDF8

 .END
