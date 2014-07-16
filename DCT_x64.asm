section .code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;16 pels / macroblock. Data type is int16
%define TEXTURE_BUFFER_STRIDE 2*16
%macro NMOVDQA 2  
	pshufd	%1, %2, 0xE4 
;	movdqa	%1, %2
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


%define a0	[esp + 0*16]
%define a1	[esp + 1*16]
%define a2	[esp + 2*16]
%define a3	[esp + 3*16]
%define a4	[esp + 4*16]
%define a5	[esp + 5*16]
%define a6	[esp + 6*16]
%define a7	[esp + 7*16]
%define b0	[esp + 8*16]
%define b1	[esp + 9*16]
%define b2	[esp + 10*16]
%define b3	[esp + 11*16]
%define b4	[esp + 12*16]
%define b5	[esp + 13*16]
%define b6	[esp + 14*16]
%define b7	[esp + 15*16]

%macro DCT_8x8_1D 0 
		movdqa			xmm0, [RCX + 0*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm1, [RCX + 1*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm2, [RCX + 2*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm3, [RCX + 3*TEXTURE_BUFFER_STRIDE]

		NMOVDQA 		xmm4, xmm0
		NMOVDQA 		xmm5, xmm1
		NMOVDQA 		xmm6, xmm2
		NMOVDQA 		xmm7, xmm3

		paddsw			xmm0, [RCX + 7*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm1, [RCX + 6*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm2, [RCX + 5*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm3, [RCX + 4*TEXTURE_BUFFER_STRIDE]

		psubsw			xmm4, [RCX + 7*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm5, [RCX + 6*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm6, [RCX + 5*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm7, [RCX + 4*TEXTURE_BUFFER_STRIDE]			;xmm0-xmm7: a[0-7]

		movdqa			a0, xmm0
		movdqa			a1, xmm1
		movdqa			a2, xmm2
		movdqa			a3, xmm3
		movdqa			a4, xmm4
		movdqa			a5, xmm5
		movdqa			a6, xmm6
		movdqa			a7, xmm7

		paddsw			xmm0, xmm3
		paddsw			xmm1, xmm2
		movdqa			xmm2, a0
		movdqa			xmm3, a1
		psubsw			xmm2, a3
		psubsw			xmm3, a2					;xmm0-xmm3: b[0-3]; xmm4-xmm7: a[4-7]

		movdqa			b0, xmm0
		movdqa			b1, xmm1
		movdqa			b2, xmm2
		movdqa			b3, xmm3

		paddsw			xmm5, xmm6
		psraw			xmm4, 1
		paddsw			xmm4, a4
		paddsw			xmm5, xmm4										
		movdqa			b4, xmm5					;b0: b[4]
		
		movdqa			xmm4, a4
		psubsw			xmm4, xmm7
		psraw			xmm6, 1
		paddsw			xmm6, a6
		psubsw			xmm4, xmm6
		movdqa			b5, xmm4					;b1: b[5]

		movdqa			xmm4, a4
		paddsw			xmm4, xmm7
		movdqa			xmm5, a5
		psraw			xmm5, 1
		paddsw			xmm5, a5
		psubsw			xmm4, xmm5
		movdqa			b6, xmm4					;b2: b[6]

		movdqa			xmm5, a5
		movdqa			xmm6, a6
		psubsw			xmm5, xmm6
		psraw			xmm7, 1
		paddsw			xmm7, a7
		paddsw			xmm5, xmm7
		movdqa			b7, xmm5					;b3: b[7]

		paddsw			xmm0, xmm1
		movdqa			[RCX + 0*TEXTURE_BUFFER_STRIDE], xmm0			;finish line0
		psraw			xmm3, 1
		paddsw			xmm3, xmm2
		movdqa			[RCX + 2*TEXTURE_BUFFER_STRIDE], xmm3			;finish line2
		movdqa			xmm0, b0
		psubsw			xmm0, xmm1
		movdqa			[RCX + 4*TEXTURE_BUFFER_STRIDE], xmm0			;finish line4
		movdqa			xmm3, b3
		psraw			xmm2, 1
		psubsw			xmm2, xmm3
		movdqa			[RCX + 6*TEXTURE_BUFFER_STRIDE], xmm2			;finish line6

		movdqa			xmm4, b4
		movdqa			xmm7, b7
		psraw			xmm7, 2
		paddsw			xmm4, xmm7
		movdqa			[RCX + 1*TEXTURE_BUFFER_STRIDE], xmm4			;finish line1
		movdqa			xmm5, b5
		movdqa			xmm6, b6
		psraw			xmm6, 2
		paddsw			xmm6, xmm5
		movdqa			[RCX + 3*TEXTURE_BUFFER_STRIDE], xmm6			;finish line3
		movdqa			xmm6, b6
		psraw			xmm5, 2
		psubsw			xmm6, xmm5
		movdqa			[RCX + 5*TEXTURE_BUFFER_STRIDE], xmm6			;finish line5
		movdqa			xmm4, b4
		movdqa			xmm7, b7
		psraw			xmm4, 2
		psubsw			xmm4, xmm7
		movdqa			[RCX + 7*TEXTURE_BUFFER_STRIDE], xmm4			;finish line7
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


%define a01b01c01d01	[esp + 0*16]
%define a23b23c23d23	[esp + 1*16]
%define a45b45c45d45	[esp + 2*16]
%define a67b67c67d67	[esp + 3*16]

%macro TRANSPOSE_8x8_x32 0 

	movdqa		xmm0, [RCX + 0*TEXTURE_BUFFER_STRIDE]
	movdqa		xmm1, [RCX + 1*TEXTURE_BUFFER_STRIDE]	
	movdqa		xmm2, [RCX + 2*TEXTURE_BUFFER_STRIDE]
	movdqa		xmm3, [RCX + 3*TEXTURE_BUFFER_STRIDE]

	NMOVDQA		xmm4, xmm0
	NMOVDQA		xmm5, xmm2

	;__m128i a03b03 = _mm_unpacklo_epi16(a, b);
	punpcklwd   xmm0, xmm1
	;__m128i a47b47 = _mm_unpackhi_epi16(a, b);
	punpckhwd   xmm4, xmm1
	;__m128i c03d03 = _mm_unpacklo_epi16(c, d);
	punpcklwd   xmm2, xmm3
	;__m128i c47d47 = _mm_unpackhi_epi16(c, d);
	punpckhwd   xmm5, xmm3
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	NMOVDQA		xmm1, xmm0
	NMOVDQA		xmm3, xmm4
	;__m128i a01b01c01d01 = _mm_unpacklo_epi32(a03b03, c03d03);
	punpckldq   xmm0, xmm2
	;__m128i a23b23c23d23 = _mm_unpackhi_epi32(a03b03, c03d03);
	punpckhdq   xmm1, xmm2
	;__m128i a45b45c45d45 = _mm_unpacklo_epi32(a47b47, c47d47);
	punpckldq   xmm4, xmm5
	;__m128i a67b67c67d67 = _mm_unpackhi_epi32(a47b47, c47d47);
	punpckhdq   xmm3, xmm5
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movdqa		a01b01c01d01, xmm0
	movdqa		a23b23c23d23, xmm1
	movdqa		a45b45c45d45, xmm4
	movdqa		a67b67c67d67, xmm3
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movdqa		xmm0, [RCX + 4*TEXTURE_BUFFER_STRIDE]
	movdqa		xmm1, [RCX + 5*TEXTURE_BUFFER_STRIDE]	
	movdqa		xmm2, [RCX + 6*TEXTURE_BUFFER_STRIDE]
	movdqa		xmm3, [RCX + 7*TEXTURE_BUFFER_STRIDE]

	NMOVDQA		xmm4, xmm0
	NMOVDQA		xmm5, xmm2

	;__m128i e03f03 = _mm_unpacklo_epi16(e, f);
	punpcklwd   xmm0, xmm1
	;__m128i e47f47 = _mm_unpackhi_epi16(e, f);
	punpckhwd   xmm4, xmm1
	;__m128i g03h03 = _mm_unpacklo_epi16(g, h);
	punpcklwd   xmm2, xmm3
	;__m128i g47h47 = _mm_unpackhi_epi16(g, h);
	punpckhwd   xmm5, xmm3

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	NMOVDQA		xmm1, xmm0
	NMOVDQA		xmm3, xmm4
	;__m128i e01f01g01h01 = _mm_unpacklo_epi32(e03f03, g03h03);
	punpckldq   xmm0, xmm2
	;__m128i e23f23g23h23 = _mm_unpackhi_epi32(e03f03, g03h03);
	punpckhdq   xmm1, xmm2
	;__m128i e45f45g45h45 = _mm_unpacklo_epi32(e47f47, g47h47);
	punpckldq   xmm4, xmm5
	;__m128i e67f67g67h67 = _mm_unpackhi_epi32(e47f47, g47h47);
	punpckhdq   xmm3, xmm5
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movdqa		xmm2, a01b01c01d01
	movdqa		xmm5, a23b23c23d23
	NMOVDQA		xmm6, xmm2
	NMOVDQA		xmm7, xmm5
	;__m128i a0b0c0d0e0f0g0h0 = _mm_unpacklo_epi64(a01b01c01d01, e01f01g01h01);
	punpcklqdq  xmm2, xmm0
	;__m128i a1b1c1d1e1f1g1h1 = _mm_unpackhi_epi64(a01b01c01d01, e01f01g01h01);
	punpckhqdq	xmm6, xmm0
	;__m128i a2b2c2d2e2f2g2h2 = _mm_unpacklo_epi64(a23b23c23d23, e23f23g23h23);
	punpcklqdq  xmm5, xmm1
	;__m128i a3b3c3d3e3f3g3h3 = _mm_unpackhi_epi64(a23b23c23d23, e23f23g23h23);
	punpckhqdq  xmm7, xmm1
	movdqa		[RCX + 0*TEXTURE_BUFFER_STRIDE], xmm2
	movdqa		[RCX + 1*TEXTURE_BUFFER_STRIDE], xmm6
	movdqa		[RCX + 2*TEXTURE_BUFFER_STRIDE], xmm5
	movdqa		[RCX + 3*TEXTURE_BUFFER_STRIDE], xmm7
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movdqa		xmm0, a45b45c45d45
	movdqa		xmm1, a67b67c67d67
	NMOVDQA		xmm2, xmm0
	NMOVDQA		xmm6, xmm1
	;__m128i a4b4c4d4e4f4g4h4 = _mm_unpacklo_epi64(a45b45c45d45, e45f45g45h45);
	punpcklqdq  xmm0, xmm4
	;__m128i a5b5c5d5e5f5g5h5 = _mm_unpackhi_epi64(a45b45c45d45, e45f45g45h45);
	punpckhqdq	xmm2, xmm4
	;__m128i a6b6c6d6e6f6g6h6 = _mm_unpacklo_epi64(a67b67c67d67, e67f67g67h67);
	punpcklqdq  xmm1, xmm3
	;__m128i a7b7c7d7e7f7g7h7 = _mm_unpackhi_epi64(a67b67c67d67, e67f67g67h67);
	punpckhqdq  xmm6, xmm3
	movdqa		[RCX + 4*TEXTURE_BUFFER_STRIDE], xmm0
	movdqa		[RCX + 5*TEXTURE_BUFFER_STRIDE], xmm2
	movdqa		[RCX + 6*TEXTURE_BUFFER_STRIDE], xmm1
	movdqa		[RCX + 7*TEXTURE_BUFFER_STRIDE], xmm6
%endmacro


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro IDCT_8x8_1D 0 
		movdqa			xmm0, [RCX + 0*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm2, [RCX + 2*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm6, [RCX + 6*TEXTURE_BUFFER_STRIDE]
		NMOVDQA 		xmm4, xmm0
		;a0
		paddsw			xmm0, [RCX + 4*TEXTURE_BUFFER_STRIDE] 
		;a4
		psubsw			xmm4, [RCX + 4*TEXTURE_BUFFER_STRIDE] 
		;a2
		psraw			xmm2, 1
		psubsw			xmm2, [RCX + 6*TEXTURE_BUFFER_STRIDE]
		;a6
		psraw			xmm6, 1
		paddsw			xmm6, [RCX + 2*TEXTURE_BUFFER_STRIDE]

		NMOVDQA 		xmm1, xmm0
		NMOVDQA 		xmm3, xmm4
		;b0 = a0 + a6
		paddsw			xmm0, xmm6
		;b6 = a0 - a6
		psubsw			xmm1, xmm6
		;b2 = a4 + a2
		paddsw			xmm4, xmm2
		;b4 = a4 - a2
		psubsw			xmm3, xmm2

		movdqa			b0, xmm0
		movdqa			b6, xmm1
		movdqa			b2, xmm4
		movdqa			b4, xmm3

		movdqa			xmm1, [RCX + 1*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm3, [RCX + 3*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm5, [RCX + 5*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm7, [RCX + 7*TEXTURE_BUFFER_STRIDE]

		NMOVDQA 		xmm6, xmm7
		NMOVDQA 		xmm4, xmm5
		NMOVDQA 		xmm2, xmm3
		NMOVDQA 		xmm0, xmm1

		;a1
		psraw			xmm6, 1
		psubsw			xmm4, xmm3
		psubsw			xmm4, xmm7
		psubsw			xmm4, xmm6
		;a3
		psraw			xmm2, 1
		paddsw			xmm0, xmm7
		NMOVDQA 		xmm6, xmm5
		paddsw			xmm2, xmm3
		psubsw			xmm0, xmm2
		;a5
		psraw			xmm6, 1
		paddsw			xmm7, xmm5
		NMOVDQA 		xmm2, xmm1
		psubsw			xmm6, xmm1
		paddsw			xmm7, xmm6
		;a7
		psraw			xmm2, 1
		paddsw			xmm1, xmm3
		paddsw			xmm2, xmm5
		paddsw			xmm2, xmm1

		NMOVDQA 		xmm1, xmm4 ;a1
		NMOVDQA 		xmm6, xmm2 ;a7
		NMOVDQA 		xmm3, xmm0 ;a3
		NMOVDQA 		xmm5, xmm7 ;a5

		psraw			xmm4, 2
		psraw			xmm2, 2
		psraw			xmm0, 2
		psraw			xmm7, 2
		;b7=-a1/4+a7
		psubsw			xmm6, xmm4
		;b1=a1+a7/4
		paddsw			xmm2, xmm1
		;b5=a3/4-a5
		psubsw			xmm0, xmm5
		;b3=a3+a5/4
		paddsw			xmm7, xmm3

		movdqa			xmm1, b0
		movdqa			xmm3, b0
		movdqa			xmm4, b6
		movdqa			xmm5, b6
		paddsw			xmm1, xmm6	;l0=b0+b7
		psubsw			xmm3, xmm6	;l7=b0-b7
		paddsw			xmm4, xmm2	;l3=b6+b1
		psubsw			xmm5, xmm2	;l4=b6-b1
		movdqa			[RCX + 0*TEXTURE_BUFFER_STRIDE], xmm1
		movdqa			[RCX + 7*TEXTURE_BUFFER_STRIDE], xmm3
		movdqa			[RCX + 3*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[RCX + 4*TEXTURE_BUFFER_STRIDE], xmm5
		movdqa			xmm4, b2
		movdqa			xmm2, b2
		movdqa			xmm1, b4
		movdqa			xmm3, b4
		paddsw			xmm4, xmm0	;l1=b2+b5
		psubsw			xmm2, xmm0	;l6=b2-b5
		paddsw			xmm1, xmm7	;l2=b4+b3
		psubsw			xmm3, xmm7	;l5=b4-b3
		movdqa			[RCX + 1*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[RCX + 6*TEXTURE_BUFFER_STRIDE], xmm2
		movdqa			[RCX + 2*TEXTURE_BUFFER_STRIDE], xmm1
		movdqa			[RCX + 5*TEXTURE_BUFFER_STRIDE], xmm3
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;void Block_8x8_DCT( short *input );
;RCX - address1
global Block_8x8_DCT_Aligned_Stride16_YASM
Block_8x8_DCT_Aligned_Stride16_YASM:

;push ebx
;push esi
;push edi


 movdqu	[RSP-16],xmm5		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm7		; in x64 we need to save MM5:MM15
 
lea RDX, [esp]					; backup stack pointer
sub esp, 13*TEXTURE_BUFFER_STRIDE					; allocate size for 16 XMM registers
and esp, 0FFFFFFF0h				; make sure it is 16 byte aligned

; do the DCT
TRANSPOSE_8x8_x32
DCT_8x8_1D
TRANSPOSE_8x8_x32
DCT_8x8_1D

lea esp, [RDX]					; restore stack


 movdqu	xmm5,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm6,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-48]		; in x64 we need to save MM5:MM15

;pop edi
;pop esi
;pop ebx

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;void Block_8x8_IDCT( short *input );
global Block_8x8_IDCT_Aligned_Stride16_YASM
Block_8x8_IDCT_Aligned_Stride16_YASM:

;push ebx
;push esi
;push edi

 movdqu	[RSP-16],xmm5		; in x64 we need to save MM5:MM15
 movdqu	[RSP-32],xmm6		; in x64 we need to save MM5:MM15
 movdqu	[RSP-48],xmm7		; in x64 we need to save MM5:MM15

lea RDX, [esp]					; backup stack pointer
sub esp, 13*TEXTURE_BUFFER_STRIDE					; allocate size for 16 XMM registers
and esp, 0FFFFFFF0h				; make sure it is 16 byte aligned

; do the DCT
TRANSPOSE_8x8_x32
IDCT_8x8_1D
TRANSPOSE_8x8_x32
IDCT_8x8_1D

lea esp, [RDX]					; restore stack


 movdqu	xmm5,[RSP-16]		; in x64 we need to save MM5:MM15
 movdqu	xmm6,[RSP-32]		; in x64 we need to save MM5:MM15
 movdqu	xmm7,[RSP-48]		; in x64 we need to save MM5:MM15

;pop edi
;pop esi
;pop ebx

ret