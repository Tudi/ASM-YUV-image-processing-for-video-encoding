section .code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;_sad_16x16_sse2_YASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_16x16_sse2_YASM
_sad_16x16_sse2_YASM:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	movdqu      xmm0,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm4,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movdqu      xmm1,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movdqu      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm3,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movdqu      xmm2,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movdqu      xmm4,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	movdqu      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]	
	movdqu      xmm3,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16

	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results
	
	pop edi
	pop esi
	pop ebx
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;sad_16x8_sse2_ASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_16x8_sse2_YASM
_sad_16x8_sse2_YASM:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	movdqu      xmm0,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm4,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movdqu      xmm1,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movdqu      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	
	psadbw      xmm1,	xmm2					; SAD row 8
	
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7 + row8

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results
	
	pop edi
	pop esi
	pop ebx
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;_sad_8x16_sse2_YASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_8x16_sse2_YASM
_sad_8x16_sse2_YASM:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	movq      xmm0,	[EAX]		;load 8 bytes from addr 1
	movq      xmm1,	[EBX]		;load 8 bytes from addr 2
	movq      xmm2,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm3,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movq      xmm4,	[EAX]		;load 8 bytes from addr 1
	movq      xmm5,	[EBX]		;load 8 bytes from addr 2
	movq      xmm6,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm7,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movq      xmm1,	[EAX]		;load 8 bytes from addr 1
	movq      xmm3,	[EBX]		;load 8 bytes from addr 2
	movq      xmm5,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm7,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movq      xmm3,	[EAX]		;load 8 bytes from addr 1
	movq      xmm7,	[EBX]		;load 8 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movq      xmm1,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm2,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movq      xmm3,	[EAX]		;load 8 bytes from addr 1
	movq      xmm4,	[EBX]		;load 8 bytes from addr 2
	movq      xmm5,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm6,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movq      xmm2,	[EAX]		;load 8 bytes from addr 1
	movq      xmm4,	[EBX]		;load 8 bytes from addr 2
	movq      xmm6,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm7,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movq      xmm4,	[EAX]		;load 8 bytes from addr 1
	movq      xmm7,	[EBX]		;load 8 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	movq      xmm1,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm2,	[EBX+EDX]	;load 8 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]	
	movq      xmm3,	[EAX]		;load 8 bytes from addr 1
	movq      xmm4,	[EBX]		;load 8 bytes from addr 2
	movq      xmm5,	[EAX+ECX]	;load 8 bytes from addr 1
	movq      xmm6,	[EBX+EDX]	;load 8 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16

	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15

	pextrw      EAX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	
	pop edi
	pop esi
	pop ebx
ret




global _sad_16x16_sse2_YASM_2
_sad_16x16_sse2_YASM_2:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	LEA			ESI, [3*ecx]
	LEA			EDI, [3*edx]

	movdqu      xmm0,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	
	movdqu      xmm4,	[EAX + 2*ECX]		;load 16 bytes from addr 1
	movdqu      xmm5,	[EBX + 2*EDX]		;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX + ESI]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX + EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	movdqu      xmm1,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm3,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	[EAX + 2*ECX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX + 2*EDX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	movdqu      xmm1,	[EAX+ESI]	;load 16 bytes from addr 1
	movdqu      xmm2,	[EBX+EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	movdqu      xmm3,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movdqu      xmm2,	[EAX+2*ECX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX+2*EDX]		;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX+ESI]	;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX+EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movdqu      xmm4,	[EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	movdqu      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	movdqu      xmm3,	[EAX+2*ECX]		;load 16 bytes from addr 1
	movdqu      xmm4,	[EBX+2*EDX]		;load 16 bytes from addr 2
	movdqu      xmm5,	[EAX+ESI]	;load 16 bytes from addr 1
	movdqu      xmm6,	[EBX+EDI]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16

	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results
	
	pop edi
	pop esi
	pop ebx
ret
