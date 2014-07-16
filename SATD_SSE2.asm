.586p	 
.mmx
.XMM
.model flat,C
option casemap :none
.DATA
.CODE

SATD4x4_ASM_SSE2 PROC C USES EBX, addr1:PTR BYTE, stride1:DWORD, addr2:PTR BYTE, stride2:DWORD
        mov       edx, addr1                       	
        mov       ecx, addr2                       
        mov       eax, stride1                     
        mov       EBX, stride2                     
		
        pxor      xmm4, xmm4                                  
        movd      xmm6, DWORD PTR [edx]                       
        movd      xmm7, DWORD PTR [ecx]                       
        movd      xmm5, DWORD PTR [edx+eax]                   
        movd      xmm0, DWORD PTR [ecx+EBX]                   

        punpcklbw xmm6, xmm4                                  
        punpcklbw xmm7, xmm4                                  
        punpcklbw xmm5, xmm4                                  
        punpcklbw xmm0, xmm4                                  
        
        psubw     xmm6, xmm7                                  
        psubw     xmm5, xmm0                                  
        
        lea       edx, DWORD PTR [edx+eax*2]                  
        lea       ecx, DWORD PTR [ecx+EBX*2]                  
        
        movd      xmm3, DWORD PTR [edx]                       
        movd      xmm0, DWORD PTR [ecx]                       
        movd      xmm2, DWORD PTR [edx+eax]                   
        movd      xmm1, DWORD PTR [ecx+EBX]                   
        
        punpcklbw xmm3, xmm4                                  
        punpcklbw xmm0, xmm4                                  
        punpcklbw xmm2, xmm4                                  
        punpcklbw xmm1, xmm4                                  
        
        psubw     xmm2, xmm1                                  
        psubw     xmm3, xmm0                                  
       
        movdqa    xmm1, xmm6                                  
        paddsw    xmm6, xmm5                                  
        psubsw    xmm1, xmm5                                  
        movdqa    xmm0, xmm3                                  
        paddsw    xmm3, xmm2                                  
        psubsw    xmm0, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm6, xmm3                                  
        psubsw    xmm2, xmm3                                  
        movdqa    xmm3, xmm1                                  
        punpcklwd xmm6, xmm2                                  
        psubsw    xmm1, xmm0                                  
        paddsw    xmm3, xmm0                                  
        punpcklwd xmm1, xmm3                                  
        movdqa    xmm3, xmm6                                  
        punpckldq xmm6, xmm1                                  
        movdqa    xmm0, xmm6                                  
        punpckhdq xmm3, xmm1                                  
        movdqa    xmm1, xmm6                                  
        movdqa    xmm2, xmm3                                  
        psrldq    xmm1, 8                                     
        psubsw    xmm0, xmm1                                  
        psrldq    xmm2, 8                                     
        paddsw    xmm6, xmm1                                  
        movdqa    xmm1, xmm3                                  
        paddsw    xmm3, xmm2                                  
        psubsw    xmm1, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm6, xmm3                                  
        psubsw    xmm2, xmm3                                  
        movdqa    xmm3, xmm0                                  
        punpcklwd xmm6, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm3, xmm1                                  
        psubsw    xmm0, xmm1                                  
        punpcklwd xmm0, xmm3                                  
        punpckhdq xmm2, xmm0                                  
        punpckldq xmm6, xmm0                                  
        movdqa    xmm0, xmm2                                  
        movdqa    xmm1, xmm6                                  
        psraw     xmm0, 15                                    
        pxor      xmm2, xmm0                                  
        psraw     xmm1, 15                                    
        pxor      xmm6, xmm1                                  
        psubw     xmm2, xmm0                                  
        psubw     xmm6, xmm1                                  
        paddw     xmm6, xmm2                                  
        movdqa    xmm0, xmm6                                  
        psrldq    xmm0, 8                                     
        paddw     xmm6, xmm0                                  
        movdqa    xmm0, xmm6                                  
        psrldq    xmm0, 4                                     
        paddw     xmm6, xmm0    
                                       
        movd      edx, xmm6                                   
        mov       eax, edx
        and		  EAX, 65535
        shr       edx, 16
        add       eax, edx
        shr       eax, 1
		
        ret                                                   

SATD4x4_ASM_SSE2 ENDP
 
SATD4x4_ASM_SSE2_CACHED PROC C addr1:PTR BYTE, addr2:PTR BYTE
        mov       edx, addr1                       	
        mov       ecx, addr2                       
		
        pxor      xmm4, xmm4                                  
        movd      xmm6, DWORD PTR [edx]                       
        movd      xmm7, DWORD PTR [ecx]                       
        movd      xmm5, DWORD PTR [edx+16]                   
        movd      xmm0, DWORD PTR [ecx+16]                   

        punpcklbw xmm6, xmm4                                  
        punpcklbw xmm7, xmm4                                  
        punpcklbw xmm5, xmm4                                  
        punpcklbw xmm0, xmm4                                  
        
        psubw     xmm6, xmm7                                  
        psubw     xmm5, xmm0                                  
        
        movd      xmm3, DWORD PTR [edx+32]                       
        movd      xmm0, DWORD PTR [ecx+32]                       
        movd      xmm2, DWORD PTR [edx+48]                   
        movd      xmm1, DWORD PTR [ecx+48]                   
        
        punpcklbw xmm3, xmm4                                  
        punpcklbw xmm0, xmm4                                  
        punpcklbw xmm2, xmm4                                  
        punpcklbw xmm1, xmm4                                  
        
        psubw     xmm2, xmm1                                  
        psubw     xmm3, xmm0                                  
       
        movdqa    xmm1, xmm6                                  
        paddsw    xmm6, xmm5                                  
        psubsw    xmm1, xmm5                                  
        movdqa    xmm0, xmm3                                  
        paddsw    xmm3, xmm2                                  
        psubsw    xmm0, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm6, xmm3                                  
        psubsw    xmm2, xmm3                                  
        movdqa    xmm3, xmm1                                  
        punpcklwd xmm6, xmm2                                  
        psubsw    xmm1, xmm0                                  
        paddsw    xmm3, xmm0                                  
        punpcklwd xmm1, xmm3                                  
        movdqa    xmm3, xmm6                                  
        punpckldq xmm6, xmm1                                  
        movdqa    xmm0, xmm6                                  
        punpckhdq xmm3, xmm1                                  
        movdqa    xmm1, xmm6                                  
        movdqa    xmm2, xmm3                                  
        psrldq    xmm1, 8                                     
        psubsw    xmm0, xmm1                                  
        psrldq    xmm2, 8                                     
        paddsw    xmm6, xmm1                                  
        movdqa    xmm1, xmm3                                  
        paddsw    xmm3, xmm2                                  
        psubsw    xmm1, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm6, xmm3                                  
        psubsw    xmm2, xmm3                                  
        movdqa    xmm3, xmm0                                  
        punpcklwd xmm6, xmm2                                  
        movdqa    xmm2, xmm6                                  
        paddsw    xmm3, xmm1                                  
        psubsw    xmm0, xmm1                                  
        punpcklwd xmm0, xmm3                                  
        punpckhdq xmm2, xmm0                                  
        punpckldq xmm6, xmm0                                  
        movdqa    xmm0, xmm2                                  
        movdqa    xmm1, xmm6                                  
        psraw     xmm0, 15                                    
        pxor      xmm2, xmm0                                  
        psraw     xmm1, 15                                    
        pxor      xmm6, xmm1                                  
        psubw     xmm2, xmm0                                  
        psubw     xmm6, xmm1                                  
        paddw     xmm6, xmm2                                  
        movdqa    xmm0, xmm6                                  
        psrldq    xmm0, 8                                     
        paddw     xmm6, xmm0                                  
        movdqa    xmm0, xmm6                                  
        psrldq    xmm0, 4                                     
        paddw     xmm6, xmm0    
                                       
        movd      edx, xmm6                                   
        mov       eax, edx
        and		  EAX, 65535
        shr       edx, 16
        add       eax, edx
        shr       eax, 1
		
        ret                                                   

SATD4x4_ASM_SSE2_CACHED ENDP
 
END
