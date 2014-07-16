const pw_32,       times 8 dw 32

[global _x264_pixel_avg_weight_w16_ssse3]
 _x264_pixel_avg_weight_w16_ssse3:
 movzx ecx, byte [esp + 0 + 28]	; alpha blend value(weight), should be relative to 64
 mov eax, 16	;row loop counter
 ;mov edx, 64
 ;sub edx, ecx
 ;shl edx, 8
 ;add ecx, edx
 movd xmm3, ecx
 ;movdqa xmm4, [pw_32]
 pshuflw xmm3, xmm3, (0)*0x55
 punpcklqdq xmm3, xmm3
 push ebx
 push esi
 push edi
 push ebp
 mov ecx, [esp + 16 + 4]	; res buff
 mov edx, [esp + 16 + 8]	; res buff stride
 mov ebx, [esp + 16 + 12]	; ADDR1
 mov esi, [esp + 16 + 16]	; stride 1
 mov edi, [esp + 16 + 20]	; ADDR2
 mov ebp, [esp + 16 + 24]	; stride 2
.height_loop:
 movq xmm0, [ebx+0]			; load addr1 8 bytes
 movq xmm1, [edi+0]			; load addr2 8 bytes
 punpcklbw xmm0, xmm1		; store in xmm0 low xmm1
 pmaddubsw xmm0, xmm3		; alpha blend 
 ;paddw xmm0, xmm4			; roundup by adding 64 / 2
 psraw xmm0, 6				; shift left 6 bits = divide by 64
 
 movq xmm6, [ebx+0+16/2]	; load addr1+8 8 bytes
 movq xmm1, [edi+0+16/2]	; load addr2+8 8 bytes
 punpcklbw xmm6, xmm1
 pmaddubsw xmm6, xmm3
 ;paddw xmm6, xmm4
 psraw xmm6, 6
 packuswb xmm0, xmm6
 movdqa [ecx+0], xmm0
 movq xmm6, [ebx+0+1*esi]
 movq xmm1, [edi+0+1*ebp]
 punpcklbw xmm6, xmm1
 pmaddubsw xmm6, xmm3
 ;paddw xmm6, xmm4
 psraw xmm6, 6
 movq xmm0, [ebx+0+1*esi+16/2]
 movq xmm1, [edi+0+1*ebp+16/2]
 punpcklbw xmm0, xmm1
 pmaddubsw xmm0, xmm3
 ;paddw xmm0, xmm4
 psraw xmm0, 6
 packuswb xmm6, xmm0
 movdqa [ecx+0+1*edx], xmm6
 sub eax, 2
 lea edi, [edi+ebp*2*1]
 lea ebx, [ebx+esi*2*1]
 lea ecx, [ecx+edx*2*1]
 jg .height_loop
 pop ebp
 pop edi
 pop esi
 pop ebx
 ret
 
 