default rel
hmul_4p:		times 2 db 1, 1, 1, 1, 1, -1, 1, -1
hmul_8p:		times 8 db 1
				times 4 db 1, -1
mask_10:		times 4 dw 0, -1
_x264_pw_1:		times 8 dw 1

section .code

;UINT satd_4x4_sse3_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
;ecx - address1
;edx - stride1
;r8 - address2
;r9 - stride2

global satd_4x4_sse3_YASM
satd_4x4_sse3_YASM:

lea R10, [3*RDX]
lea R11, [3*R9]


movd xmm2, [R8]
movd xmm4, [R8+R9]
movlhps xmm2, xmm4
movsldup xmm2, xmm2
movd xmm3, [R8+2*R9]
movd xmm4, [R8+R11]
movlhps xmm3, xmm4
movsldup xmm3, xmm3

movd xmm0, [RCX]
movd xmm4, [RCX+RDX]
movlhps xmm0, xmm4
movsldup xmm0, xmm0
movd xmm1, [RCX+2*RDX]
movd xmm4, [RCX+R10]
movlhps xmm1, xmm4
movsldup xmm1, xmm1

movaps xmm4, [hmul_4p]

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
movd RAX, xmm0
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _satd_8x8_sse3_YASM_prepared
_satd_8x8_sse3_YASM_prepared:

 movddup xmm4, [R8]
 movddup xmm5, [R8+R9]
 movddup xmm0, [RCX]
 movddup xmm1, [RCX+RDX]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5
 movddup xmm4, [R8+2*R9]
 movddup xmm5, [R8+R11]
 movddup xmm2, [RCX+2*RDX]
 movddup xmm3, [RCX+R10]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea RCX, [RCX+4*RDX]
 lea R8, [R8+4*R9]
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

 movddup xmm4, [R8]
 movddup xmm5, [R8+R9]
 movddup xmm0, [RCX]
 movddup xmm1, [RCX+RDX]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm0, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm1, xmm7
 psubw xmm0, xmm4
 psubw xmm1, xmm5
 movddup xmm4, [R8+2*R9]
 movddup xmm5, [R8+R11]
 movddup xmm2, [RCX+2*RDX]
 movddup xmm3, [RCX+R10]
 pmaddubsw xmm4, xmm7
 pmaddubsw xmm2, xmm7
 pmaddubsw xmm5, xmm7
 pmaddubsw xmm3, xmm7
 psubw xmm2, xmm4
 psubw xmm3, xmm5
 lea RCX, [RCX+4*RDX]
 lea R8, [R8+4*R9]
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
;RCX - address1
;RDX - stride1
;R8 - address2
;R9 - stride2
global satd_8x8_sse3_YASM
satd_8x8_sse3_YASM:

 movdqu	[RSP-16],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm7		; in x64 we need to save MM5:MM15
 
 movaps xmm7, [hmul_8p]
 lea R10, [3*RDX]
 lea R11, [3*R9]
 pxor xmm6, xmm6
 call _satd_8x8_sse3_YASM_prepared
 pmaddwd xmm6, [_x264_pw_1]
 movhlps xmm7, xmm6
 paddd xmm6, xmm7
 pshuflw xmm7, xmm6, 0xE
 paddd xmm6, xmm7
 movd RAX, xmm6
 
 movdqu	xmm6,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-32]		; in x64 we need to save MM5:MM15

 ret
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
global satd_16x16_sse3_YASM
satd_16x16_sse3_YASM:

 movdqu	[RSP-16],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm7		; in x64 we need to save MM5:MM15
 
 mov	[RSP-48],RCX		
 mov	[RSP-64],R8		
 
 movaps xmm7, [hmul_8p]
 lea R10, [3*RDX]
 lea R11, [3*R9]
 pxor xmm6, xmm6
 call _satd_8x8_sse3_YASM_prepared
 call _satd_8x8_sse3_YASM_prepared
 
 mov	RCX,[RSP-48]	
 mov	R8,	[RSP-64]		
 
 add RCX, 8
 add R8, 8
 call _satd_8x8_sse3_YASM_prepared
 call _satd_8x8_sse3_YASM_prepared
 pmaddwd xmm6, [_x264_pw_1]
 movhlps xmm7, xmm6
 paddd xmm6, xmm7
 pshuflw xmm7, xmm6, 0xE
 paddd xmm6, xmm7
 movd RAX, xmm6
 
 movdqu	xmm6,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-32]		; in x64 we need to save MM5:MM15

 ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;