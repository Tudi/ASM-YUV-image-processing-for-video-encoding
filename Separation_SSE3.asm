;Separate16x16_SSE3_YASM(BYTE * src, INT src_stride, BYTE * pred, INT pred_stride, SHORT * texture, INT texture_stride);

%macro SeparationBlock 2 
	LDDQU xmm0, [eax+%1]
	LDDQU xmm1, [ebx+%2]
	;MOVDQA xmm2, xmm0
	pshufd xmm2, xmm0, 0xE4 ; REPLACING movdqa xmm2, xmm0
	;MOVDQA xmm3, xmm1
	pshufd xmm3, xmm1, 0xE4 ; REPLACING movdqa xmm2, xmm0
	PUNPCKHBW xmm2, xmm4
	PUNPCKHBW xmm3, xmm4
	PUNPCKLBW xmm0, xmm4
	PUNPCKLBW xmm1, xmm4
	PSUBW xmm0, xmm1
	PSUBW xmm2, xmm3
	MOVDQU [edi], xmm0
	MOVDQU [edi+16], xmm2

	lea edi, [edi+esi*2]
%endmacro

%macro SeparationBlock2Row 0 
SeparationBlock 0,0
SeparationBlock ecx,edx
lea eax, [eax+2*ecx]
lea ebx, [ebx+2*edx]
%endmacro

%macro SeparationBlock4Row 0 
SeparationBlock2Row
SeparationBlock2Row
%endmacro

%macro SeparationBlock8Row 0 
SeparationBlock4Row
SeparationBlock4Row
%endmacro

global _Separate16x16_SSE3_YASM
_Separate16x16_SSE3_YASM:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ; src
mov ecx, [esp + 12 + 4 + 1*4] ; src_stride
mov ebx, [esp + 12 + 4 + 2*4] ; pred
mov edx, [esp + 12 + 4 + 3*4] ; pred_stride
mov edi, [esp + 12 + 4 + 4*4] ; texture
mov esi, [esp + 12 + 4 + 5*4] ; texture_stride

PXOR xmm4, xmm4

SeparationBlock8Row
SeparationBlock8Row

pop edi
pop esi
pop ebx

ret
