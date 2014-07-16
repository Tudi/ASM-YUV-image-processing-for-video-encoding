section .code

%macro SSDBlock1Row 2 
	LDDQU xmm0, [eax+%1]
	LDDQU xmm1, [ebx+%2]
	;MOVDQA xmm5, xmm0
	pshufd xmm5, xmm0, 0xE4 ;
	PMAXUB xmm0, xmm1
	PMINUB xmm1, xmm5
	;PSUBUSB xmm0, xmm1
	PSUBB xmm0, xmm1
	;MOVDQA xmm1, xmm0
	pshufd xmm1, xmm0, 0xE4 ;
	PUNPCKLBW xmm0, xmm6
	PUNPCKHBW xmm1, xmm6
	PMADDWD xmm0, xmm0
	PMADDWD xmm1, xmm1
	PADDD xmm4, xmm0
	PADDD xmm4, xmm1
%endmacro

%macro SSDBlock4Row 0 
	SSDBlock1Row 0,0
	SSDBlock1Row ecx,edx
	SSDBlock1Row ecx*2,edx*2
	SSDBlock1Row edi,esi
	lea eax, [eax+4*ecx]
	lea ebx, [ebx+4*edx]
%endmacro

;int ssd_16x16_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
global _ssd_16x16_sse3_YASM
_ssd_16x16_sse3_YASM:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ;address1
mov ecx, [esp + 12 + 4 + 1*4] ;stride1
mov ebx, [esp + 12 + 4 + 2*4] ;address2
mov edx, [esp + 12 + 4 + 3*4] ;stride2

lea esi, [3*edx]
lea edi, [3*ecx]

PXOR xmm4, xmm4
PXOR xmm6, xmm6 ;zero

SSDBlock4Row
SSDBlock4Row
SSDBlock4Row

SSDBlock1Row 0,0
SSDBlock1Row ecx,edx
SSDBlock1Row ecx*2,edx*2
SSDBlock1Row edi,esi


PHADDD xmm4,xmm6
PHADDD xmm4,xmm6
movd EAX, xmm4

pop edi
pop esi
pop ebx

ret

;unsigned int sum_16x16_sse3_YASM( unsigned char *address, unsigned int stride);
global _sum_16x16_sse3_YASM
_sum_16x16_sse3_YASM:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ;address
mov ecx, [esp + 12 + 4 + 1*4] ;stride

lea edx, [3*ecx]

PXOR xmm4, xmm4
PXOR xmm1, xmm1

LDDQU xmm0, [eax]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+edx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea eax, [eax+4*ecx]

LDDQU xmm0, [eax]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+edx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea eax, [eax+4*ecx]

LDDQU xmm0, [eax]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+edx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea eax, [eax+4*ecx]

LDDQU xmm0, [eax]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+ecx*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [eax+edx]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0


movdqu xmm1, xmm4
psrldq xmm1, 8
paddw xmm4, xmm1
movd EAX, xmm4

pop edi
pop esi
pop ebx

ret

;_sad_16x16_sse3_YASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_16x16_sse3_YASM
_sad_16x16_sse3_YASM:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	LDDQU      xmm0,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm1,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	LDDQU      xmm4,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm5,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	LDDQU      xmm1,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	LDDQU      xmm3,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	LDDQU      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	LDDQU      xmm3,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	LDDQU      xmm2,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	LDDQU      xmm4,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	LDDQU      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]	
	LDDQU      xmm3,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
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

;sad_16x8_sse3_ASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_16x8_sse3_YASM
_sad_16x8_sse3_YASM:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	LDDQU      xmm0,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm1,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	LDDQU      xmm4,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm5,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm6,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	LDDQU      xmm1,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	LDDQU      xmm3,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	LDDQU      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
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

;_sad_8x16_sse3_YASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

global _sad_8x16_sse3_YASM
_sad_8x16_sse3_YASM:

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




global _sad_16x16_sse3_YASM_2
_sad_16x16_sse3_YASM_2:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	LEA			ESI, [3*ecx]
	LEA			EDI, [3*edx]

	LDDQU      xmm0,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm1,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm2,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX+EDX]	;load 16 bytes from addr 2
	
	LDDQU      xmm4,	[EAX + 2*ECX]		;load 16 bytes from addr 1
	LDDQU      xmm5,	[EBX + 2*EDX]		;load 16 bytes from addr 2
	LDDQU      xmm6,	[EAX + ESI]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX + EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4

	LDDQU      xmm1,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	LDDQU      xmm3,	[EAX + 2*ECX]		;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX + 2*EDX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7

	LDDQU      xmm1,	[EAX+ESI]	;load 16 bytes from addr 1
	LDDQU      xmm2,	[EBX+EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	LDDQU      xmm3,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm6,	[EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	LDDQU      xmm2,	[EAX+2*ECX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX+2*EDX]		;load 16 bytes from addr 2
	LDDQU      xmm6,	[EAX+ESI]	;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX+EDI]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	LDDQU      xmm4,	[EAX]		;load 16 bytes from addr 1
	LDDQU      xmm7,	[EBX]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13

	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13

	LDDQU      xmm1,	[EAX+ECX]	;load 16 bytes from addr 1
	LDDQU      xmm2,	[EBX+EDX]	;load 16 bytes from addr 2
	LDDQU      xmm3,	[EAX+2*ECX]		;load 16 bytes from addr 1
	LDDQU      xmm4,	[EBX+2*EDX]		;load 16 bytes from addr 2
	LDDQU      xmm5,	[EAX+ESI]	;load 16 bytes from addr 1
	LDDQU      xmm6,	[EBX+EDI]	;load 16 bytes from addr 2
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


%macro SASBlock4Row 0 
	LDDQU      xmm0,	[EAX]			;load 16 bytes from addr 1
	LDDQU      xmm1,	[EBX]			;load 16 bytes from addr 2
	LDDQU      xmm2,	[EAX + ECX]		;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX + EDX]		;load 16 bytes from addr 2
	LDDQU      xmm4,	[EAX + 2*ECX]	;load 16 bytes from addr 1
	LDDQU      xmm5,	[EBX + 2*EDX]	;load 16 bytes from addr 2
	psadbw     xmm0,	xmm1			; SAD row 1
	psadbw     xmm2,	xmm3			; SAD row 2
	psadbw     xmm4,	xmm5			; SAD row 3
	LDDQU      xmm1,	[EAX + ESI]		;load 16 bytes from addr 1
	LDDQU      xmm3,	[EBX + EDI]		;load 16 bytes from addr 2
	paddusw    xmm0,	xmm2			; row1 + row2
	paddusw    xmm7,	xmm4			; store + row 3
	psadbw     xmm1,	xmm3			; SAD row 4
	paddusw    xmm7,	xmm0			; store ( row1 + row2 ) + row 3
	paddusw    xmm7,	xmm1			; store row4
	LEA			EAX,	[EAX + 4*ECX]
	LEA			EBX,	[EBX + 4*EDX]
%endmacro

global _sad_16x16_sse3_YASM2
_sad_16x16_sse3_YASM2:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov ecx, [esp + 12 + 4 + 1*4]
	mov ebx, [esp + 12 + 4 + 2*4]
	mov edx, [esp + 12 + 4 + 3*4]

	lea esi, [3*ecx]
	lea edi, [3*ecx]

	PXOR xmm7, xmm7

	SASBlock4Row
	SASBlock4Row
	SASBlock4Row
	SASBlock4Row

	pextrw      EAX,	xmm7,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm7,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results
	
	pop edi
	pop esi
	pop ebx
ret