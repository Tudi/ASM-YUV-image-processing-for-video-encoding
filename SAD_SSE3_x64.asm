
global sad_16x8_sse3_YASM
sad_16x8_sse3_YASM:

lea R10, [3*RDX]
lea R11, [3*R9]

PXOR xmm4, xmm4

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

movdqu	xmm1, xmm4
psrldq 	xmm1, 8
paddw	xmm4, xmm1
movd RAX, xmm4
ret

global sad_8x16_sse3_YASM
sad_8x16_sse3_YASM:

lea R10, [3*RDX]
lea R11, [3*R9]

PXOR xmm4, xmm4

MOVLPS xmm0, [R8]
MOVHPS xmm0, [R8+R9]
MOVLPS xmm1, [RCX]
MOVHPS xmm1, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

MOVLPS xmm0, [R8+R9*2]
MOVHPS xmm0, [R8+R11]
MOVLPS xmm1, [RCX+RDX*2]
MOVHPS xmm1, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

MOVLPS xmm0, [R8]
MOVHPS xmm0, [R8+R9]
MOVLPS xmm1, [RCX]
MOVHPS xmm1, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

MOVLPS xmm0, [R8+R9*2]
MOVHPS xmm0, [R8+R11]
MOVLPS xmm1, [RCX+RDX*2]
MOVHPS xmm1, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

MOVLPS xmm0, [R8]
MOVHPS xmm0, [R8+R9]
MOVLPS xmm1, [RCX]
MOVHPS xmm1, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

MOVLPS xmm0, [R8+R9*2]
MOVHPS xmm0, [R8+R11]
MOVLPS xmm1, [RCX+RDX*2]
MOVHPS xmm1, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

MOVLPS xmm0, [R8]
MOVHPS xmm0, [R8+R9]
MOVLPS xmm1, [RCX]
MOVHPS xmm1, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

MOVLPS xmm0, [R8+R9*2]
MOVHPS xmm0, [R8+R11]
MOVLPS xmm1, [RCX+RDX*2]
MOVHPS xmm1, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

movdqu	xmm1, xmm4
psrldq 	xmm1, 8
paddw	xmm4, xmm1
movd RAX, xmm4
ret



global ssd_16x16_sse3_YASM
ssd_16x16_sse3_YASM:

lea R10, [3*RDX]
lea R11, [3*R9]

PXOR xmm4, xmm4
PXOR xmm3, xmm3		;zero

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9]
LDDQU xmm1, [RCX+RDX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R11]
LDDQU xmm1, [RCX+R10]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9]
LDDQU xmm1, [RCX+RDX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R11]
LDDQU xmm1, [RCX+R10]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9]
LDDQU xmm1, [RCX+RDX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R11]
LDDQU xmm1, [RCX+R10]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9]
LDDQU xmm1, [RCX+RDX]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

LDDQU xmm0, [R8+R11]
LDDQU xmm1, [RCX+R10]
MOVDQU	xmm2, xmm0
PMAXUB xmm0, xmm1
PMINUB xmm1, xmm2
PSUBUSB xmm0, xmm1
MOVDQU xmm1, xmm0
PUNPCKLBW xmm0, xmm3
PUNPCKHBW xmm1, xmm3
PMADDWD xmm0, xmm0
PMADDWD xmm1, xmm1
PADDD xmm4, xmm0
PADDD xmm4, xmm1

PHADDD xmm4,xmm3
PHADDD xmm4,xmm3
movd RAX, xmm4

ret


global sad_16x16_sse3_YASM
sad_16x16_sse3_YASM:

lea R10, [3*RDX]
lea R11, [3*R9]

PXOR xmm4, xmm4

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

lea R8, [R8+4*R9]
lea RCX, [RCX+4*RDX]

LDDQU xmm0, [R8]
LDDQU xmm1, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R9]
LDDQU xmm3, [RCX+RDX]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

LDDQU xmm0, [R8+R9*2]
LDDQU xmm1, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm2, [R8+R11]
LDDQU xmm3, [RCX+R10]
PSADBW xmm2, xmm3
PADDW xmm4, xmm2

movdqu	xmm1, xmm4
psrldq 	xmm1, 8
paddw	xmm4, xmm1
movd RAX, xmm4
ret


global sum_16x16_sse3_YASM
sum_16x16_sse3_YASM:

lea R10, [3*RDX]

PXOR xmm4, xmm4
PXOR xmm1, xmm1

LDDQU xmm0, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea RCX, [RCX+4*RDX]

LDDQU xmm0, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea RCX, [RCX+4*RDX]

LDDQU xmm0, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

lea RCX, [RCX+4*RDX]

LDDQU xmm0, [RCX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+RDX*2]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0

LDDQU xmm0, [RCX+R10]
PSADBW xmm0, xmm1
PADDW xmm4, xmm0


movdqu	xmm1, xmm4
psrldq 	xmm1, 8
paddw	xmm4, xmm1
movd RAX, xmm4
ret
