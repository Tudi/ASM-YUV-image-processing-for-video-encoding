; void read_luma_inter_pred_avg_16x16_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; void read_luma_inter_pred_avg_16x16_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);

global _read_luma_inter_pred_avg_16x16_SSE3
_read_luma_inter_pred_avg_16x16_SSE3:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ; src addr 1
mov ecx, [esp + 12 + 4 + 1*4] ; src addr 2
mov ebx, [esp + 12 + 4 + 2*4] ; src stride
mov edx, [esp + 12 + 4 + 3*4] ; dst addr
mov edi, [esp + 12 + 4 + 4*4] ; dst stride

lea esi, [ ebx * 3 ]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6
lea edx, [edx+2*edi]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6
lea edx, [edx+2*edi]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6
lea edx, [edx+2*edi]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6

pop edi
pop esi
pop ebx
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


global _read_luma_inter_pred_avg_16x8_SSE3
_read_luma_inter_pred_avg_16x8_SSE3:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ; src addr 1
mov ecx, [esp + 12 + 4 + 1*4] ; src addr 2
mov ebx, [esp + 12 + 4 + 2*4] ; src stride
mov edx, [esp + 12 + 4 + 3*4] ; dst addr
mov edi, [esp + 12 + 4 + 4*4] ; dst stride

lea esi, [ ebx * 3 ]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6
lea edx, [edx+2*edi]

LDDQU xmm0, [eax]		;row0
LDDQU xmm1, [ecx]	
LDDQU xmm2, [eax+ebx]	;row1
LDDQU xmm3, [ecx+ebx]
LDDQU xmm4, [eax+2*ebx]	;row2
LDDQU xmm5, [ecx+2*ebx]	
LDDQU xmm6, [eax+esi]	;row3
LDDQU xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
MOVDQU [edx], xmm0
MOVDQU [edx+edi], xmm2
lea edx, [edx+2*edi]
MOVDQU [edx], xmm4
MOVDQU [edx+edi], xmm6
lea edx, [edx+2*edi]

pop edi
pop esi
pop ebx

ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _read_luma_inter_pred_avg_8x16_SSE3
_read_luma_inter_pred_avg_8x16_SSE3:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ; src addr 1
mov ebx, [esp + 12 + 4 + 2*4] ; src stride
mov ecx, [esp + 12 + 4 + 1*4] ; src addr 2
mov edx, [esp + 12 + 4 + 3*4] ; dst addr
mov edi, [esp + 12 + 4 + 4*4] ; src stride

lea esi, [ ebx * 3 ]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6
lea edx, [edx+2*edi]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6
lea edx, [edx+2*edi]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6
lea edx, [edx+2*edi]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6

pop edi
pop esi
pop ebx
ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _read_luma_inter_pred_avg_8x8_SSE3
_read_luma_inter_pred_avg_8x8_SSE3:

push ebx
push esi
push edi

mov eax, [esp + 12 + 4 + 0*4] ; src addr 1
mov ebx, [esp + 12 + 4 + 2*4] ; src stride
mov ecx, [esp + 12 + 4 + 1*4] ; src addr 2
mov edx, [esp + 12 + 4 + 3*4] ; dst addr
mov edi, [esp + 12 + 4 + 4*4] ; src stride

lea esi, [ ebx * 3 ]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6
lea edx, [edx+2*edi]

movq xmm0, [eax]		;row0
movq xmm1, [ecx]	
movq xmm2, [eax+ebx]	;row1
movq xmm3, [ecx+ebx]
movq xmm4, [eax+2*ebx]	;row2
movq xmm5, [ecx+2*ebx]	
movq xmm6, [eax+esi]	;row3
movq xmm7, [ecx+esi]
lea eax, [eax+4*ebx]
lea ecx, [ecx+4*ebx]
PAVGB xmm0, xmm1
PAVGB xmm2, xmm3
PAVGB xmm4, xmm5
PAVGB xmm6, xmm7
movq [edx], xmm0
movq [edx+edi], xmm2
lea edx, [edx+2*edi]
movq [edx], xmm4
movq [edx+edi], xmm6
lea edx, [edx+2*edi]

pop edi
pop esi
pop ebx
ret
