.586p	 
.mmx
.XMM
.model flat,C
option casemap :none
.DATA
.CODE

sad_16x16_sse2_ASM PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

	mov EAX, addr1
	LEA EAX, [EAX]
	mov EBX, addr2
	LEA EBX, [EBX]
	mov ECX, stride1
	mov EDX, stride2

	movdqu      xmm0,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm2,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm4,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm5,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4
	
	movdqu      xmm1,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7
	
	movdqu      xmm1,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm3,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm6,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movdqu      xmm2,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movdqu      xmm4,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13
	
	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13
	
	movdqu      xmm1,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]	
	movdqu      xmm3,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm6,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16
	
	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15
	
	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results

	ret

sad_16x16_sse2_ASM ENDP

SAD16x8_ASM_SSE2 PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD

	mov EAX, addr1
	LEA EAX, [EAX]
	mov EBX, addr2
	LEA EBX, [EBX]
	mov ECX, stride1
	mov EDX, stride2

	movdqu      xmm0,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm2,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	movdqu      xmm4,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm5,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm6,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4
	
	movdqu      xmm1,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	movdqu      xmm5,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	LEA			EAX,	[EAX + 2*ECX]
	LEA			EBX,	[EBX + 2*EDX]
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6

	movdqu      xmm3,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [EBX]		;load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7
	
	movdqu      xmm1,	xmmword ptr [EAX+ECX]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [EBX+EDX]	;load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 8
	
	paddusw     xmm0,	xmm1					; row8 + row9
	
	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results

	ret

SAD16x8_ASM_SSE2 ENDP

SAD16x16_ASM_SSE2_CACHED PROC C addr1:PTR BYTE, addr2:PTR BYTE

	mov EAX, addr1
	LEA EAX, [EAX]
	mov ECX, addr2
	LEA ECX, [ECX]
	
	movdqa      xmm0,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	xmmword ptr [ECX]		;load 16 bytes from addr 2
	movdqa      xmm2,	xmmword ptr [EAX+16*1]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [ECX+16*3*1];load 16 bytes from addr 2
	movdqa      xmm4,	xmmword ptr [EAX+16*2]	;load 16 bytes from addr 1
	movdqu      xmm5,	xmmword ptr [ECX+16*3*2];load 16 bytes from addr 2
	movdqa      xmm6,	xmmword ptr [EAX+16*3]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*3];load 16 bytes from addr 2
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4
	movdqa      xmm1,	xmmword ptr [EAX+16*4]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [ECX+16*3*4];load 16 bytes from addr 2
	movdqa      xmm5,	xmmword ptr [EAX+16*5]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*5];load 16 bytes from addr 2
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6
	movdqa      xmm3,	xmmword ptr [EAX+16*6]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*6];load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7
	
	movdqa      xmm1,	xmmword ptr [EAX+16*7]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [ECX+16*3*7];load 16 bytes from addr 2
	movdqa      xmm3,	xmmword ptr [EAX+16*8]	;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [ECX+16*3*8];load 16 bytes from addr 2
	movdqa      xmm5,	xmmword ptr [EAX+16*9]	;load 16 bytes from addr 1
	movdqu      xmm6,	xmmword ptr [ECX+16*3*9];load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 8
	psadbw      xmm3,	xmm4					; SAD row 9
	psadbw      xmm5,	xmm6					; SAD row 10
	movdqa      xmm2,	xmmword ptr [EAX+16*10]	;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [ECX+16*3*10];load 16 bytes from addr 2
	movdqa      xmm6,	xmmword ptr [EAX+16*11]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*11];load 16 bytes from addr 2
	psadbw      xmm2,	xmm4					; SAD row 11
	psadbw      xmm6,	xmm7					; SAD row 12
	movdqa      xmm4,	xmmword ptr [EAX+16*12]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*12];load 16 bytes from addr 2
	psadbw      xmm4,	xmm7					; SAD row 13
	
	paddusw     xmm1,	xmm3					; row8 + row9
	paddusw     xmm2,	xmm5					; row10 + row11
	paddusw     xmm4,	xmm6					; row12 + row13

	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9
	paddusw     xmm2,	xmm4					; row10 + row11 + row12 + row13
	paddusw     xmm0,	xmm2					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13
	
	movdqa      xmm1,	xmmword ptr [EAX+16*13]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [ECX+16*3*13];load 16 bytes from addr 2
	movdqa      xmm3,	xmmword ptr [EAX+16*14]	;load 16 bytes from addr 1
	movdqu      xmm4,	xmmword ptr [ECX+16*3*14];load 16 bytes from addr 2
	movdqa      xmm5,	xmmword ptr [EAX+16*15]	;load 16 bytes from addr 1
	movdqu      xmm6,	xmmword ptr [ECX+16*3*15];load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 14
	psadbw      xmm3,	xmm4					; SAD row 15
	psadbw      xmm5,	xmm6					; SAD row 16
	
	paddusw     xmm1,	xmm3					; row14 + row15
	paddusw     xmm0,	xmm5					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row8 + row9 + row10 + row11 + row12 + row13 + row16 + row14 + row15
	
	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results

	ret

SAD16x16_ASM_SSE2_CACHED ENDP

SAD16x8_ASM_SSE2_CACHED PROC C addr1:PTR BYTE, addr2:PTR BYTE

	mov EAX, addr1
	LEA EAX, [EAX]
	mov ECX, addr2
	LEA ECX, [ECX]
	
	movdqa      xmm0,	xmmword ptr [EAX]		;load 16 bytes from addr 1
	movdqu      xmm1,	xmmword ptr [ECX]		;load 16 bytes from addr 2
	movdqa      xmm2,	xmmword ptr [EAX+16*1]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [ECX+16*3*1];load 16 bytes from addr 2
	movdqa      xmm4,	xmmword ptr [EAX+16*2]	;load 16 bytes from addr 1
	movdqu      xmm5,	xmmword ptr [ECX+16*3*2];load 16 bytes from addr 2
	movdqa      xmm6,	xmmword ptr [EAX+16*3]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*3];load 16 bytes from addr 2
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4
	movdqa      xmm1,	xmmword ptr [EAX+16*4]	;load 16 bytes from addr 1
	movdqu      xmm3,	xmmword ptr [ECX+16*3*4];load 16 bytes from addr 2
	movdqa      xmm5,	xmmword ptr [EAX+16*5]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*5];load 16 bytes from addr 2
	psadbw      xmm1,	xmm3					; SAD row 5
	psadbw      xmm5,	xmm7					; SAD row 6
	movdqa      xmm3,	xmmword ptr [EAX+16*6]	;load 16 bytes from addr 1
	movdqu      xmm7,	xmmword ptr [ECX+16*3*6];load 16 bytes from addr 2
	psadbw      xmm3,	xmm7					; SAD row 7

	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm1,	xmm5					; row5 + row6
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	paddusw     xmm1,	xmm3					; row5 + row6 + row7
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7
	
	movdqa      xmm1,	xmmword ptr [EAX+16*7]	;load 16 bytes from addr 1
	movdqu      xmm2,	xmmword ptr [ECX+16*3*7];load 16 bytes from addr 2
	psadbw      xmm1,	xmm2					; SAD row 8
	paddusw     xmm0,	xmm1					; row1 + row2 + row3 + row4 + row5 + row6 + row7 + row8

	pextrw      EAX,	xmm0,	4 				;unpack the higher 4 bytes of the SAD result
	pextrw      ECX,	xmm0,	0				;unpack the lower 4 bytes of the SAD result
	add         EAX,	ECX						;sum up the 2 partial results

	ret

SAD16x8_ASM_SSE2_CACHED ENDP

END
