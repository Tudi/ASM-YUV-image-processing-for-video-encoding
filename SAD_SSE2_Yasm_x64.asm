section .code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;_sad_16x16_sse2_YASM PROC C USES R8, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD
;ecx - address1
;edx - stride1
;r8 - address2
;r9 - stride2

global sad_16x16_sse2_YASM
sad_16x16_sse2_YASM:

 movdqu	[RSP-16],xmm5		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm7		; in x64 we need to save MM5:MM15

	movdqu      xmm0,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm2,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[R8+R9]		;load 16 bytes from addr 2
	
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	movdqu      xmm4,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm6,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movdqu      xmm1,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm3,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm5,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[R8]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movdqu      xmm1,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	movdqu      xmm3,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm5,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm6,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movdqu      xmm2,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm6,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movdqu      xmm4,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[R8]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	movdqu      xmm1,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]	
	movdqu      xmm3,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm5,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm6,	[R8+R9]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16

	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      EDX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	EDX						;sum up the 2 partial results
	
 movdqu	xmm5,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm6,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-48]		; in x64 we need to save MM5:MM15
 
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;sad_16x8_sse2_ASM PROC C USES R8, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global sad_16x8_sse2_YASM
sad_16x8_sse2_YASM:
 movdqu	[RSP-16],xmm5		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm7		; in x64 we need to save MM5:MM15

	movdqu      xmm0,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm2,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	movdqu      xmm4,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm6,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movdqu      xmm1,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm3,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm5,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[R8]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movdqu      xmm1,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[R8+R9]	;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	
	psadbw      xmm1,	xmm2					; SAD row 8
	
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7 + row8

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      EDX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	EDX						;sum up the 2 partial results
 movdqu	xmm5,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm6,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-48]		; in x64 we need to save MM5:MM15
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;_sad_8x16_sse2_YASM PROC C USES R8, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global sad_8x16_sse2_YASM
sad_8x16_sse2_YASM:
 movdqu	[RSP-16],xmm5		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm7		; in x64 we need to save MM5:MM15
	movq      xmm0,	[ECX]		;load 8 bytes from addr 1
	movq      xmm1,	[R8]		;load 8 bytes from addr 2
	movq      xmm2,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm3,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	movq      xmm4,	[ECX]		;load 8 bytes from addr 1
	movq      xmm5,	[R8]		;load 8 bytes from addr 2
	movq      xmm6,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm7,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movq      xmm1,	[ECX]		;load 8 bytes from addr 1
	movq      xmm3,	[R8]		;load 8 bytes from addr 2
	movq      xmm5,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm7,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movq      xmm3,	[ECX]		;load 8 bytes from addr 1
	movq      xmm7,	[R8]		;load 8 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movq      xmm1,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm2,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	movq      xmm3,	[ECX]		;load 8 bytes from addr 1
	movq      xmm4,	[R8]		;load 8 bytes from addr 2
	movq      xmm5,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm6,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movq      xmm2,	[ECX]		;load 8 bytes from addr 1
	movq      xmm4,	[R8]		;load 8 bytes from addr 2
	movq      xmm6,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm7,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movq      xmm4,	[ECX]		;load 8 bytes from addr 1
	movq      xmm7,	[R8]		;load 8 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	movq      xmm1,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm2,	[R8+R9]	;load 8 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,	[R8 + 2*R9]	
	movq      xmm3,	[ECX]		;load 8 bytes from addr 1
	movq      xmm4,	[R8]		;load 8 bytes from addr 2
	movq      xmm5,	[ECX+EDX]	;load 8 bytes from addr 1
	movq      xmm6,	[R8+R9]	;load 8 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16

	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15

	pextrw      EAX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
 movdqu	xmm5,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm6,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-48]		; in x64 we need to save MM5:MM15
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro SAD_4x16 0
	movdqu      xmm0,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm2,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	movdqu      xmm4,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm6,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	
	paddusw		xmm8,	xmm0
	paddusw     xmm8,	xmm4					
%endmacro

global sad_16x16_sse2_YASM_2
sad_16x16_sse2_YASM_2:

 movdqu	[RSP-16],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm7		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm8		; in x64 we need to save MM5:MM15

	PXOR		xmm8,	xmm8
	
	SAD_4x16
	SAD_4x16
	SAD_4x16
	SAD_4x16

	pextrw      EAX,	xmm8,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      EDX,	xmm8,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	EDX						;sum up the 2 partial results
	
 movdqu	xmm6,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm8,[RSP-48]		; in x64 we need to save MM5:MM15

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro SAD_2x16 0
	movdqu      xmm0,	[ECX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[R8]		;load 16 bytes from addr 2
	movdqu      xmm2,	[ECX+EDX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[R8+R9]		;load 16 bytes from addr 2
	LEA			ECX,	[ECX + 2*EDX]
	LEA			R8,		[R8 + 2*R9]
	psadbw      xmm0,	xmm1					; SAD row 1
	paddusw		xmm4,	xmm0
	psadbw      xmm2,	xmm3					; SAD row 2
	paddusw     xmm4,	xmm2					; row1 + row2
%endmacro

global sad_16x16_sse2_YASM_3
sad_16x16_sse2_YASM_3:

	PXOR		xmm4,	xmm4

	SAD_2x16
	SAD_2x16
	SAD_2x16
	SAD_2x16
	
	SAD_2x16
	SAD_2x16
	SAD_2x16
	SAD_2x16

	pextrw      EAX,	xmm4,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      EDX,	xmm4,	0				;unpack the lower 4 bytes of the SAD result
	
	add         EAX,	EDX						;sum up the 2 partial results
ret