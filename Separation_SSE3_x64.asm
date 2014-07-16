global Separate16x16_SSE3_YASM
Separate16x16_SSE3_YASM:

SUB RSP, 38h

PXOR xmm4, xmm4
MOV RAX, [RSP+60h]
MOV R10D, dword[RSP+68h]
MOV R11D, 10h

Pos:

LDDQU xmm0, [RCX]
LDDQU xmm1, [R8]
MOVDQU xmm2, xmm0
MOVDQU xmm3, xmm1
PUNPCKHBW xmm2, xmm4
PUNPCKHBW xmm3, xmm4
PUNPCKLBW xmm0, xmm4
PUNPCKLBW xmm1, xmm4
PSUBW xmm0, xmm1
PSUBW xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+16], xmm2

lea RCX, [RCX+RDX]
lea R8, [R8+R9]
lea RAX, [RAX+R10*2]

SUB R11, 01h
JNZ Pos

ADD RSP, 38h
ret
