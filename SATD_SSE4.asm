hmul_4p:		times 2 db 1, 1, 1, 1, 1, -1, 1, -1
hmul_8p:		times 8 db 1
				times 4 db 1, -1
mask_10:		times 4 dw 0, -1
_x264_pw_1:		times 8 dw 1

section .code

;UINT satd_4x4_sse4( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );

global _satd_4x4_sse4_YASM
_satd_4x4_sse4_YASM:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4]
mov ecx, [esp + 12 + 4 + 1*4]
mov edx, [esp + 12 + 4 + 2*4]
mov ebx, [esp + 12 + 4 + 3*4]

lea esi, [3*ecx]
lea edi, [3*ebx]

movdqa xmm4, [hmul_4p]

movd xmm2, [edx]
movd xmm5, [edx+ebx]
shufps xmm2, xmm5, 0

movd xmm3, [edx+2*ebx]
movd xmm5, [edx+edi]
shufps xmm3, xmm5, 0

movd xmm0, [eax]
movd xmm5, [eax+ecx]
shufps xmm0, xmm5, 0

movd xmm1, [eax+2*ecx]
movd xmm5, [eax+esi]
shufps xmm1, xmm5, 0

pmaddubsw xmm2, xmm4
pmaddubsw xmm0, xmm4
pmaddubsw xmm3, xmm4
pmaddubsw xmm1, xmm4

psubw xmm0, xmm2
psubw xmm1, xmm3
movdqa xmm2, xmm0
paddw xmm0, xmm1
psubw xmm1, xmm2
movdqa xmm2, xmm0
punpcklqdq xmm0, xmm1
punpckhqdq xmm2, xmm1
movdqa xmm1, xmm0
paddw xmm0, xmm2
psubw xmm2, xmm1
movdqa xmm1, xmm0
pblendw xmm0, xmm2, 10101010b
pslld xmm2, 16
psrld xmm1, 16
por xmm2, xmm1
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

global _satd_8x8_sse4_YASM_prepared
_satd_8x8_sse4_YASM_prepared:

 movq xmm4, [edx]
 movq xmm5, [edx+ebx]
 punpcklqdq xmm4, xmm4
 movddup xmm0, [eax]
 punpcklqdq xmm5, xmm5
 movddup xmm1, [eax+ecx]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5

 movq xmm4, [edx+2*ebx]
 movq xmm5, [edx+edi]
 punpcklqdq xmm4, xmm4
 movddup xmm2, [eax+2*ecx]
 punpcklqdq xmm5, xmm5
 movddup xmm3, [eax+esi]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea eax, [eax+4*ecx]
 lea edx, [edx+4*ebx]
 movdqa xmm4, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm4
 movdqa xmm4, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm4
 movdqa xmm4, xmm0
 paddw xmm0, xmm2
 psubw xmm2, xmm4
 movdqa xmm4, xmm1
 paddw xmm1, xmm3
 psubw xmm3, xmm4

 pabsw xmm0, xmm0
 pabsw xmm2, xmm2
 pabsw xmm1, xmm1
 pabsw xmm3, xmm3

 movdqa xmm4, xmm0
 pblendw xmm0, xmm2, 10101010b
 pslld xmm2, 16
 psrld xmm4, 16
 por xmm2, xmm4
 pmaxsw xmm0, xmm2
 paddw xmm6, xmm0

 movdqa xmm4, xmm1
 pblendw xmm1, xmm3, 10101010b
 pslld xmm3, 16
 psrld xmm4, 16
 por xmm3, xmm4
 pmaxsw xmm1, xmm3
 paddw xmm6, xmm1
pixel_satd_8x4_internal_sse4:

 movq xmm4, [edx]
 movq xmm5, [edx+ebx]
 punpcklqdq xmm4, xmm4
 movddup xmm0, [eax]
 punpcklqdq xmm5, xmm5
 movddup xmm1, [eax+ecx]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5

 movq xmm4, [edx+2*ebx]
 movq xmm5, [edx+edi]
 punpcklqdq xmm4, xmm4
 movddup xmm2, [eax+2*ecx]
 punpcklqdq xmm5, xmm5
 movddup xmm3, [eax+esi]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea eax, [eax+4*ecx]
 lea edx, [edx+4*ebx]
 movdqa xmm4, xmm0
 paddw xmm0, xmm1
 psubw xmm1, xmm4
 movdqa xmm4, xmm2
 paddw xmm2, xmm3
 psubw xmm3, xmm4
 movdqa xmm4, xmm0
 paddw xmm0, xmm2
 psubw xmm2, xmm4
 movdqa xmm4, xmm1
 paddw xmm1, xmm3
 psubw xmm3, xmm4

 pabsw xmm0, xmm0
 pabsw xmm2, xmm2
 pabsw xmm1, xmm1
 pabsw xmm3, xmm3

 movdqa xmm4, xmm0
 pblendw xmm0, xmm2, 10101010b
 pslld xmm2, 16
 psrld xmm4, 16
 por xmm2, xmm4
 pmaxsw xmm0, xmm2
 paddw xmm6, xmm0

 movdqa xmm4, xmm1
 pblendw xmm1, xmm3, 10101010b
 pslld xmm3, 16
 psrld xmm4, 16
 por xmm3, xmm4
 pmaxsw xmm1, xmm3
 paddw xmm6, xmm1
 ret
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _satd_8x8_sse4_YASM
_satd_8x8_sse4_YASM:
 push ebx
 push esi
 push edi
 mov eax, [esp + 12 + 4 + 0*4]
 mov ecx, [esp + 12 + 4 + 1*4]
 mov edx, [esp + 12 + 4 + 2*4]
 mov ebx, [esp + 12 + 4 + 3*4]

 movdqa xmm7, [hmul_8p]
 lea esi, [3*ecx]
 lea edi, [3*ebx]
 pxor xmm6, xmm6
 call _satd_8x8_sse4_YASM_prepared
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

global _satd_16x16_sse4_YASM
_satd_16x16_sse4_YASM:
 push ebx
 push esi
 push edi
 mov eax, [esp + 12 + 4 + 0*4]
 mov ecx, [esp + 12 + 4 + 1*4]
 mov edx, [esp + 12 + 4 + 2*4]
 mov ebx, [esp + 12 + 4 + 3*4]

 movdqa xmm7, [hmul_8p]
 lea esi, [3*ecx]
 lea edi, [3*ebx]
 pxor xmm6, xmm6
 call _satd_8x8_sse4_YASM_prepared
 call _satd_8x8_sse4_YASM_prepared
 mov eax, dword [esp + 12 + 4]
 mov edx, dword [esp + 12 + 12]
 add eax, 8
 add edx, 8
 call _satd_8x8_sse4_YASM_prepared
 call _satd_8x8_sse4_YASM_prepared
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

;UINT satd_4x4_sse4( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );

global _satd_4x4_sse4_YASM_CACHED
_satd_4x4_sse4_YASM_CACHED:

mov eax, [esp + 12 + 4 + 0*4]
mov edx, [esp + 12 + 4 + 2*4]

movdqa xmm4, [hmul_4p]

movd xmm2, [edx]
movd xmm5, [edx+16]
shufps xmm2, xmm5, 0

movd xmm3, [edx+2*16]
movd xmm5, [edx+3*16]
shufps xmm3, xmm5, 0

movd xmm0, [eax]
movd xmm5, [eax+16]
shufps xmm0, xmm5, 0

movd xmm1, [eax+2*16]
movd xmm5, [eax+3*16]
shufps xmm1, xmm5, 0

pmaddubsw xmm2, xmm4
pmaddubsw xmm0, xmm4
pmaddubsw xmm3, xmm4
pmaddubsw xmm1, xmm4

psubw xmm0, xmm2
psubw xmm1, xmm3
movdqa xmm2, xmm0
paddw xmm0, xmm1
psubw xmm1, xmm2
movdqa xmm2, xmm0
punpcklqdq xmm0, xmm1
punpckhqdq xmm2, xmm1
movdqa xmm1, xmm0
paddw xmm0, xmm2
psubw xmm2, xmm1
movdqa xmm1, xmm0
pblendw xmm0, xmm2, 10101010b
pslld xmm2, 16
psrld xmm1, 16
por xmm2, xmm1
pabsw xmm0, xmm0
pabsw xmm2, xmm2
pmaxsw xmm0, xmm2
pmaddwd xmm0, [_x264_pw_1]
movhlps xmm2, xmm0
paddd xmm0, xmm2
pshuflw xmm2, xmm0, 0xE
paddd xmm0, xmm2
movd eax, xmm0
ret
