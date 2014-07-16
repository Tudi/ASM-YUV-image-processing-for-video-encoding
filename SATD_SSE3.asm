hmul_4p:		times 2 db 1, 1, 1, 1, 1, -1, 1, -1
hmul_8p:		times 8 db 1
				times 4 db 1, -1
mask_10:		times 4 dw 0, -1
_x264_pw_1:		times 8 dw 1
mask_ac4b:				dw 0, -1, 0, -1, -1, -1, -1, -1
mask_ac8:				dw 0, -1, -1, -1, -1, -1, -1, -1
mask_1100:		times 2 dd 0, -1

section .code

;UINT satd_4x4_sse3_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );

global _satd_4x4_sse3_YASM
_satd_4x4_sse3_YASM:

push ebx
push esi
push edi
mov eax, [esp + 12 + 4 + 0*4]
mov ecx, [esp + 12 + 4 + 1*4]
mov edx, [esp + 12 + 4 + 2*4]
mov ebx, [esp + 12 + 4 + 3*4]

lea esi, [3*ecx]
lea edi, [3*ebx]

movaps xmm4, [hmul_4p]
movd xmm2, [edx]
movd xmm5, [edx+ebx]
movlhps xmm2, xmm5
movsldup xmm2, xmm2
movd xmm3, [edx+2*ebx]
movd xmm5, [edx+edi]
movlhps xmm3, xmm5
movsldup xmm3, xmm3

movd xmm0, [eax]
movd xmm5, [eax+ecx]
movlhps xmm0, xmm5
movsldup xmm0, xmm0
movd xmm1, [eax+2*ecx]
movd xmm5, [eax+esi]
movlhps xmm1, xmm5
movsldup xmm1, xmm1

pmaddubsw xmm2, xmm4
pmaddubsw xmm0, xmm4
pmaddubsw xmm3, xmm4
pmaddubsw xmm1, xmm4
psubw xmm0, xmm2
psubw xmm1, xmm3
movaps xmm2, xmm0
paddw xmm0, xmm1
psubw xmm1, xmm2
movaps xmm2, xmm0
movlhps xmm0, xmm1
punpckhqdq xmm2, xmm1
movaps xmm1, xmm0
paddw xmm0, xmm2
psubw xmm2, xmm1
movaps xmm1, [mask_10]
movaps xmm3, xmm2
pslld xmm2, 16
pand xmm3, xmm1
pandn xmm1, xmm0
psrld xmm0, 16
por xmm2, xmm1
por xmm0, xmm3
pabsw xmm0, xmm0
pabsw xmm2, xmm2
pmaxsw xmm0, xmm2
pmaddwd xmm0, [_x264_pw_1]
movhlps xmm2, xmm0
paddd xmm0, xmm2
pshuflw xmm2, xmm0, 0xE
paddd xmm0, xmm2
movd eax, xmm0
pop edi
pop esi
pop ebx
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _satd_8x8_sse3_YASM_prepared
_satd_8x8_sse3_YASM_prepared:

 movddup xmm4, [edx]
 movddup xmm5, [edx+ebx]
 movddup xmm0, [eax]
 movddup xmm1, [eax+ecx]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5
 movddup xmm4, [edx+2*ebx]
 movddup xmm5, [edx+edi]
 movddup xmm2, [eax+2*ecx]
 movddup xmm3, [eax+esi]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea eax, [eax+4*ecx]
 lea edx, [edx+4*ebx]
 movaps xmm4, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm4
 movaps xmm4, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm4
 movaps xmm4, xmm0
 paddw xmm0, xmm2
 psubw xmm2, xmm4
 movaps xmm4, xmm1
 paddw xmm1, xmm3
 psubw xmm3, xmm4

 pabsw xmm0, xmm0
 pabsw xmm2, xmm2
 pabsw xmm1, xmm1
 pabsw xmm3, xmm3

 movaps xmm4, [mask_10]
 movaps xmm5, xmm2
 pslld xmm2, 16
 pand xmm5, xmm4
 pandn xmm4, xmm0
 psrld xmm0, 16
 por xmm2, xmm4
 por xmm0, xmm5
 pmaxsw xmm0, xmm2
 paddw xmm6, xmm0

 movaps xmm4, [mask_10]
 movaps xmm5, xmm3
 pslld xmm3, 16
 pand xmm5, xmm4
 pandn xmm4, xmm1
 psrld xmm1, 16
 por xmm3, xmm4
 por xmm1, xmm5
 pmaxsw xmm1, xmm3
 paddw xmm6, xmm1
pixel_satd_8x4_internal_ssse3:

 movddup xmm4, [edx]
 movddup xmm5, [edx+ebx]
 movddup xmm0, [eax]
 movddup xmm1, [eax+ecx]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5
 movddup xmm4, [edx+2*ebx]
 movddup xmm5, [edx+edi]
 movddup xmm2, [eax+2*ecx]
 movddup xmm3, [eax+esi]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea eax, [eax+4*ecx]
 lea edx, [edx+4*ebx]
 movaps xmm4, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm4
 movaps xmm4, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm4
 movaps xmm4, xmm0
 paddw xmm0, xmm2
 psubw xmm2, xmm4
 movaps xmm4, xmm1
 paddw xmm1, xmm3
 psubw xmm3, xmm4

 pabsw xmm0, xmm0
 pabsw xmm2, xmm2
 pabsw xmm1, xmm1
 pabsw xmm3, xmm3

 movaps xmm4, [mask_10]
 movaps xmm5, xmm2
 pslld xmm2, 16
 pand xmm5, xmm4
 pandn xmm4, xmm0
 psrld xmm0, 16
 por xmm2, xmm4
 por xmm0, xmm5
 pmaxsw xmm0, xmm2
 paddw xmm6, xmm0

 movaps xmm4, [mask_10]
 movaps xmm5, xmm3
 pslld xmm3, 16
 pand xmm5, xmm4
 pandn xmm4, xmm1
 psrld xmm1, 16
 por xmm3, xmm4
 por xmm1, xmm5
 pmaxsw xmm1, xmm3
 paddw xmm6, xmm1
 ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _satd_8x8_sse3_YASM
_satd_8x8_sse3_YASM:
 push ebx
 push esi
 push edi
 mov eax, [esp + 12 + 4 + 0*4]
 mov ecx, [esp + 12 + 4 + 1*4]
 mov edx, [esp + 12 + 4 + 2*4]
 mov ebx, [esp + 12 + 4 + 3*4]

 movaps xmm7, [hmul_8p]
 lea esi, [3*ecx]
 lea edi, [3*ebx]
 pxor xmm6, xmm6
 call _satd_8x8_sse3_YASM_prepared
 pmaddwd xmm6, [_x264_pw_1]
 movhlps xmm7, xmm6
 paddd xmm6, xmm7
 pshuflw xmm7, xmm6, 0xE
 paddd xmm6, xmm7
 movd eax, xmm6
 pop edi
 pop esi
 pop ebx
 ret
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
global _satd_16x16_sse3_YASM
_satd_16x16_sse3_YASM:
 push ebx
 push esi
 push edi
 mov eax, [esp + 12 + 4 + 0*4]
 mov ecx, [esp + 12 + 4 + 1*4]
 mov edx, [esp + 12 + 4 + 2*4]
 mov ebx, [esp + 12 + 4 + 3*4]

 movaps xmm7, [hmul_8p]
 lea esi, [3*ecx]
 lea edi, [3*ebx]
 pxor xmm6, xmm6
 call _satd_8x8_sse3_YASM_prepared
 call _satd_8x8_sse3_YASM_prepared
 mov eax, dword [esp + 12 + 4]
 mov edx, dword [esp + 12 + 12]
 add eax, 8
 add edx, 8
 call _satd_8x8_sse3_YASM_prepared
 call _satd_8x8_sse3_YASM_prepared
 pmaddwd xmm6, [_x264_pw_1]
 movhlps xmm7, xmm6
 paddd xmm6, xmm7
 pshuflw xmm7, xmm6, 0xE
 paddd xmm6, xmm7
 movd eax, xmm6
 pop edi
 pop esi
 pop ebx
 ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


global _satd_4x4_sse3_YASM_CACHED
_satd_4x4_sse3_YASM_CACHED:
push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4]
mov edx, [esp + 12 + 4 + 1*4]

movaps xmm4, [hmul_4p]
movd xmm2, [edx]
movd xmm5, [edx+16]
movlhps xmm2, xmm5
movsldup xmm2, xmm2
movd xmm3, [edx+2*16]
movd xmm5, [edx+3*16]
movlhps xmm3, xmm5
movsldup xmm3, xmm3

movd xmm0, [eax]
movd xmm5, [eax+16]
movlhps xmm0, xmm5
movsldup xmm0, xmm0
movd xmm1, [eax+2*16]
movd xmm5, [eax+3*16]
movlhps xmm1, xmm5
movsldup xmm1, xmm1

pmaddubsw xmm2, xmm4
pmaddubsw xmm0, xmm4
pmaddubsw xmm3, xmm4
pmaddubsw xmm1, xmm4
psubw xmm0, xmm2
psubw xmm1, xmm3
movaps xmm2, xmm0
paddw xmm0, xmm1
psubw xmm1, xmm2
movaps xmm2, xmm0
movlhps xmm0, xmm1
punpckhqdq xmm2, xmm1
movaps xmm1, xmm0
paddw xmm0, xmm2
psubw xmm2, xmm1
movaps xmm1, [mask_10]
movaps xmm3, xmm2
pslld xmm2, 16
pand xmm3, xmm1
pandn xmm1, xmm0
psrld xmm0, 16
por xmm2, xmm1
por xmm0, xmm3
pabsw xmm0, xmm0
pabsw xmm2, xmm2
pmaxsw xmm0, xmm2
pmaddwd xmm0, [_x264_pw_1]
movhlps xmm2, xmm0
paddd xmm0, xmm2
pshuflw xmm2, xmm0, 0xE
paddd xmm0, xmm2
movd eax, xmm0
pop edi
pop esi
pop ebx
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _sad_4x4_YASM_CACHED
_sad_4x4_YASM_CACHED:

	push ebx
	push esi
	push edi

	mov eax, [esp + 12 + 4 + 0*4]
	mov edx, [esp + 12 + 4 + 1*4]
	
	movdqu      xmm0,	[EAX]					;load 16 bytes from addr 1
	movdqu      xmm1,	[ECX]					;load 16 bytes from addr 2
	movdqu      xmm2,	[EAX+16*1]				;load 16 bytes from addr 1
	movdqu      xmm3,	[ECX+16*1]				;load 16 bytes from addr 2
	movdqu      xmm4,	[EAX+16*2]				;load 16 bytes from addr 1
	movdqu      xmm5,	[ECX+16*2]				;load 16 bytes from addr 2
	movdqu      xmm6,	[EAX+16*3]				;load 16 bytes from addr 1
	movdqu      xmm7,	[ECX+16*3]				;load 16 bytes from addr 2
	psadbw      xmm0,	xmm1					; SAD row 1
	psadbw      xmm2,	xmm3					; SAD row 2
	psadbw      xmm4,	xmm5					; SAD row 3
	psadbw      xmm6,	xmm7					; SAD row 4
	
	paddusw     xmm0,	xmm2					; row1 + row2
	paddusw     xmm4,	xmm6					; row3 + row4
	paddusw     xmm0,	xmm4					; row1 + row2 + row3 + row4
	
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
[global _satd_8x8_ac_intra_ssse3_YASM_prepared]
 _satd_8x8_ac_intra_ssse3_YASM_prepared:
 movaps xmm7, [hmul_8p]
 movddup xmm0, [eax+edx]
 movddup xmm1, [eax+ecx]
 movddup xmm2, [eax+ecx*2]
 movddup xmm3, [eax]
 lea eax, [eax+ecx*4]
 pmaddubsw xmm1, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm3, xmm7
 pmaddubsw xmm2, xmm7
 movaps xmm4, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm4
 movaps xmm4, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm4
 movaps xmm4, xmm0
 paddw xmm0, xmm2
 psubw xmm2, xmm4
 movaps xmm4, xmm1
 paddw xmm1, xmm3
 psubw xmm3, xmm4
 movaps [esp+4], xmm1
 movddup xmm4, [eax+edx]
 movddup xmm5, [eax+ecx]
 movddup xmm6, [eax+ecx*2]
 movddup xmm1, [eax]
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm1, xmm7
 pmaddubsw xmm6, xmm7
 movaps xmm7, xmm4
 paddw xmm4, xmm5
 psubw xmm5, xmm7
 movaps xmm7, xmm6
 paddw xmm6, xmm1
 psubw xmm1, xmm7
 movaps xmm7, xmm4
 paddw xmm4, xmm6
 psubw xmm6, xmm7
 movaps xmm7, xmm5
 paddw xmm5, xmm1
 psubw xmm1, xmm7
 movaps xmm7, [esp+4]
 movaps [esp+4], xmm6
 movaps [esp+4+16], xmm1
 movaps xmm6, [mask_10]
 movaps xmm1, xmm7
 pslld xmm7, 16
 pand xmm1, xmm6
 pandn xmm6, xmm0
 psrld xmm0, 16
 por xmm7, xmm6
 por xmm0, xmm1
 movaps xmm6, xmm0
 paddw xmm0, xmm7
 psubw xmm7, xmm6
 movaps xmm6, [mask_10]
 movaps xmm1, xmm3
 pslld xmm3, 16
 pand xmm1, xmm6
 pandn xmm6, xmm2
 psrld xmm2, 16
 por xmm3, xmm6
 por xmm2, xmm1
 movaps xmm6, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm6
 movaps xmm6, [esp+4]
 movaps xmm1, [esp+4+16]
 movaps [esp+4], xmm7
 movaps [esp+4+16], xmm0
 movaps xmm7, [mask_10]
 movaps xmm0, xmm5
 pslld xmm5, 16
 pand xmm0, xmm7
 pandn xmm7, xmm4
 psrld xmm4, 16
 por xmm5, xmm7
 por xmm4, xmm0
 movaps xmm7, xmm4
 paddw xmm4, xmm5
 psubw xmm5, xmm7
 movaps xmm7, [mask_10]
 movaps xmm0, xmm1
 pslld xmm1, 16
 pand xmm0, xmm7
 pandn xmm7, xmm6
 psrld xmm6, 16
 por xmm1, xmm7
 por xmm6, xmm0
 movaps xmm7, xmm6
 paddw xmm6, xmm1
 psubw xmm1, xmm7
 movaps xmm0, [esp+4+16]
 movaps [esp+4+16], xmm2
 movaps [esp+4+32], xmm3
 pabsw xmm7, xmm0
 pabsw xmm2, xmm4
 pabsw xmm3, xmm5
 paddw xmm7, xmm2
 paddw xmm0, xmm4
 paddw xmm4, xmm4
 psubw xmm4, xmm0
 pand xmm7, [mask_ac4b]
 pabsw xmm2, [esp+4]
 paddw xmm7, xmm3
 pabsw xmm3, [esp+4+16]
 paddw xmm7, xmm2
 pabsw xmm2, [esp+4+32]
 paddw xmm7, xmm3
 pabsw xmm3, xmm6
 paddw xmm7, xmm2
 pabsw xmm2, xmm1
 paddw xmm7, xmm3
 movaps xmm3, xmm1
 paddw xmm7, xmm2
 movaps xmm2, xmm6
 psubw xmm1, [esp+4+32]
 paddw xmm3, [esp+4+32]
 movaps [esp+4+32], xmm7
 movaps xmm7, xmm5
 psubw xmm6, [esp+4+16]
 paddw xmm2, [esp+4+16]
 psubw xmm5, [esp+4]
 paddw xmm7, [esp+4]
 movaps [esp+4+16], xmm4
 movaps xmm4, xmm1
 psllq xmm1, 32
 pxor xmm1, xmm3
 pand xmm1, [mask_1100]
 pxor xmm3, xmm1
 psrlq xmm1, 32
 pxor xmm4, xmm1
 pabsw xmm4, xmm4
 pabsw xmm3, xmm3
 pmaxsw xmm4, xmm3
 movaps xmm3, [mask_1100]
 movaps xmm1, xmm6
 psllq xmm6, 32
 pand xmm1, xmm3
 pandn xmm3, xmm2
 psrlq xmm2, 32
 por xmm6, xmm3
 por xmm2, xmm1
 pabsw xmm2, xmm2
 pabsw xmm6, xmm6
 pmaxsw xmm2, xmm6
 movaps xmm1, [esp+4+16]
 movaps xmm6, [mask_1100]
 movaps xmm3, xmm5
 psllq xmm5, 32
 pand xmm3, xmm6
 pandn xmm6, xmm7
 psrlq xmm7, 32
 por xmm5, xmm6
 por xmm7, xmm3
 pabsw xmm7, xmm7
 pabsw xmm5, xmm5
 pmaxsw xmm7, xmm5
 movaps xmm5, [mask_1100]
 movaps xmm6, xmm1
 psllq xmm1, 32
 pand xmm6, xmm5
 pandn xmm5, xmm0
 psrlq xmm0, 32
 por xmm1, xmm5
 por xmm0, xmm6
 movaps xmm5, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm5
 paddw xmm2, xmm4
 paddw xmm2, xmm7
 paddw xmm2, xmm2
 pabsw xmm1, xmm1
 pand xmm0, [mask_ac8]
 pabsw xmm0, xmm0
 paddw xmm2, xmm1
 paddw xmm2, xmm0
 movaps [esp+4+16], xmm2
 ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[global _satd_16x16_ac_intra_ssse3_YASM]
 _satd_16x16_ac_intra_ssse3_YASM:
 mov eax, [esp + 0 + 4 + 0*4]
 mov ecx, [esp + 0 + 4 + 1*4]

 sub esp, 48+12
 lea edx, [ecx*3]
 call _satd_8x8_ac_intra_ssse3_YASM_prepared
 lea eax, [eax+ecx*4]
 sub esp, 32
 call _satd_8x8_ac_intra_ssse3_YASM_prepared
 neg edx
 sub esp, 32
 lea eax, [eax+edx*4+8*1]
 neg edx
 call _satd_8x8_ac_intra_ssse3_YASM_prepared
 lea eax, [eax+ecx*4]
 sub esp, 32
 call _satd_8x8_ac_intra_ssse3_YASM_prepared
 movaps xmm7, [esp+2*16]
 paddusw xmm2, [esp+3*16]
 paddusw xmm7, [esp+4*16]
 paddusw xmm2, [esp+5*16]
 paddusw xmm7, [esp+6*16]
 paddusw xmm2, [esp+7*16]
 paddusw xmm7, [esp+8*16]
 psrlw xmm2, 1
 movaps xmm0, xmm2
 pslld xmm2, 16
 psrld xmm0, 16
 psrld xmm2, 16
 paddd xmm2, xmm0
 movhlps xmm0, xmm2
 paddd xmm2, xmm0
 pshuflw xmm0, xmm2, 0xE
 paddd xmm2, xmm0
 pmaddwd xmm7, [_x264_pw_1]
 movhlps xmm4, xmm7
 paddd xmm7, xmm4
 pshuflw xmm4, xmm7, 0xE
 paddd xmm7, xmm4
 movd edx, xmm2
 movd eax, xmm7
 shr edx, 2 - (16*16 >> 8)
 shr eax, 1
 add esp, 16+16*16/2+12
 ret

