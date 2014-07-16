global read_luma_inter_pred_avg_16x16_SSE3
read_luma_inter_pred_avg_16x16_SSE3:

SUB RSP, 28h
MOV RAX, R9
MOV R9D, dword[RSP+50h]

lea R10, [3*R8]
lea R11, [3*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2

lea RDX, [RDX+4*R8]
lea RCX, [RCX+4*R8]
lea RAX, [RAX+4*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2

lea RDX, [RDX+4*R8]
lea RCX, [RCX+4*R8]
lea RAX, [RAX+4*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2

lea RDX, [RDX+4*R8]
lea RCX, [RCX+4*R8]
lea RAX, [RAX+4*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2


ADD RSP, 28h
ret

global read_luma_inter_pred_avg_16x8_SSE3
read_luma_inter_pred_avg_16x8_SSE3:

SUB RSP, 28h
MOV RAX, R9
MOV R9D, dword[RSP+50h]

lea R10, [3*R8]
lea R11, [3*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2

lea RDX, [RDX+4*R8]
lea RCX, [RCX+4*R8]
lea RAX, [RAX+4*R9]

LDDQU xmm0, [RDX]
LDDQU xmm1, [RCX]
LDDQU xmm2, [RDX+R8]
LDDQU xmm3, [RCX+R8]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX], xmm0
MOVDQU [RAX+R9], xmm2

LDDQU xmm0, [RDX+R8*2]
LDDQU xmm1, [RCX+R8*2]
LDDQU xmm2, [RDX+R10]
LDDQU xmm3, [RCX+R10]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
MOVDQU [RAX+R9*2], xmm0
MOVDQU [RAX+R11], xmm2

ADD RSP, 28h
ret


