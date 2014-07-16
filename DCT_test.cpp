#include "StdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>

#define MB_SIZE		16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT_LARGE	1	//to have a larger runtime
	#define LOOP_TEST_COUNT			1	//to have a larger runtime
	#define MB_X_REF	68
	#define MB_Y_REF	45
#else
	#define MB_X_REF	680
	#define MB_Y_REF	450
	#define LOOP_TEST_COUNT_LARGE	50	//to have a larger runtime
	#define LOOP_TEST_COUNT			1	//to have a larger runtime
#endif

extern "C" void Block_8x8_DCT_Aligned_Stride16_YASM(short *Buf);
extern "C" void Block_8x8_IDCT_Aligned_Stride16_YASM(short *Buf);

#ifndef _WIN64
	#ifdef _DEBUG
		#pragma comment (lib, "libs/sad_lib_D_x32_vs10.lib")
	#else
		#pragma comment (lib, "libs/sad_lib_R_x32_vs10.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment (lib, "libs/sad_lib_D_x64_vs10.lib")
	#else
		#pragma comment (lib, "libs/sad_lib_R_x64_vs10.lib")
	#endif
#endif

#define X64_COMPATIBLE
#define TEXTURE_BUFFER_STRIDE 16

void Block_8x8_DCT( short *input )
{
	
	__declspec(align(16)) int a[8],b[8];
	__declspec(align(16)) int m6[8][8];
	int i;
	for( i=0; i<8; i++)
    {
      a[0] = input[0+i*TEXTURE_BUFFER_STRIDE] + input[7+i*TEXTURE_BUFFER_STRIDE];
      a[1] = input[1+i*TEXTURE_BUFFER_STRIDE] + input[6+i*TEXTURE_BUFFER_STRIDE];
      a[2] = input[2+i*TEXTURE_BUFFER_STRIDE] + input[5+i*TEXTURE_BUFFER_STRIDE];
      a[3] = input[3+i*TEXTURE_BUFFER_STRIDE] + input[4+i*TEXTURE_BUFFER_STRIDE];
      
      a[4] = input[0+i*TEXTURE_BUFFER_STRIDE] - input[7+i*TEXTURE_BUFFER_STRIDE];
      a[5] = input[1+i*TEXTURE_BUFFER_STRIDE] - input[6+i*TEXTURE_BUFFER_STRIDE];
      a[6] = input[2+i*TEXTURE_BUFFER_STRIDE] - input[5+i*TEXTURE_BUFFER_STRIDE];
      a[7] = input[3+i*TEXTURE_BUFFER_STRIDE] - input[4+i*TEXTURE_BUFFER_STRIDE];

	  b[0] = a[0] + a[3];
      b[1] = a[1] + a[2];
      b[2] = a[0] - a[3];
      b[3] = a[1] - a[2];    

      b[4]= a[5] + a[6] + ((a[4]>>1) + a[4]);
      b[5]= a[4] - a[7] - ((a[6]>>1) + a[6]);
      b[6]= a[4] + a[7] - ((a[5]>>1) + a[5]);
      b[7]= a[5] - a[6] + ((a[7]>>1) + a[7]);
      
      m6[0][i] = b[0] + b[1];
      m6[2][i] = b[2] + (b[3]>>1);
      m6[4][i] = b[0] - b[1];
      m6[6][i] = (b[2]>>1) - b[3];
      m6[1][i] =   b[4] + (b[7]>>2);
      m6[3][i] =   b[5] + (b[6]>>2);
      m6[5][i] =   b[6] - (b[5]>>2);
      m6[7][i] = - b[7] + (b[4]>>2);
    }
    // vertical transform
    for( i=0; i<8; i++)
    {
      a[0] = m6[i][0] + m6[i][7];
      a[1] = m6[i][1] + m6[i][6];
      a[2] = m6[i][2] + m6[i][5];
      a[3] = m6[i][3] + m6[i][4];
      
      a[4] = m6[i][0] - m6[i][7];
      a[5] = m6[i][1] - m6[i][6];
      a[6] = m6[i][2] - m6[i][5];
      a[7] = m6[i][3] - m6[i][4];
      
      b[0] = a[0] + a[3];
      b[1] = a[1] + a[2];
      b[2] = a[0] - a[3];
      b[3] = a[1] - a[2];
      
      b[4]= a[5] + a[6] + ((a[4]>>1) + a[4]);
      b[5]= a[4] - a[7] - ((a[6]>>1) + a[6]);
      b[6]= a[4] + a[7] - ((a[5]>>1) + a[5]);
      b[7]= a[5] - a[6] + ((a[7]>>1) + a[7]);
      
	  input[i] = b[0] + b[1];
      input[2*TEXTURE_BUFFER_STRIDE + i] = b[2] + (b[3]>>1);
      input[4*TEXTURE_BUFFER_STRIDE + i] = b[0] - b[1];
      input[6*TEXTURE_BUFFER_STRIDE + i] = (b[2]>>1) - b[3];
      input[1*TEXTURE_BUFFER_STRIDE + i] =   b[4] + (b[7]>>2);
      input[3*TEXTURE_BUFFER_STRIDE + i] =   b[5] + (b[6]>>2);
      input[5*TEXTURE_BUFFER_STRIDE + i] =   b[6] - (b[5]>>2);
      input[7*TEXTURE_BUFFER_STRIDE + i] = - b[7] + (b[4]>>2);
    }
}

void Block_8x8_IDCT(short *input)
{
	//Copied from JM, to be optimized
	int m6[8][8];
	int a[8], b[8];
	int i;
	short *dataptr = input;

	for( i=0; i<8; i++)
    {
      a[0] = dataptr[i*TEXTURE_BUFFER_STRIDE + 0] + dataptr[i*TEXTURE_BUFFER_STRIDE + 4];
      a[4] = dataptr[i*TEXTURE_BUFFER_STRIDE + 0] - dataptr[i*TEXTURE_BUFFER_STRIDE + 4];
      a[2] = (dataptr[i*TEXTURE_BUFFER_STRIDE + 2]>>1) - dataptr[i*TEXTURE_BUFFER_STRIDE + 6];
      a[6] = dataptr[i*TEXTURE_BUFFER_STRIDE + 2] + (dataptr[i*TEXTURE_BUFFER_STRIDE + 6]>>1);
      
      b[0] = a[0] + a[6];
      b[2] = a[4] + a[2];
      b[4] = a[4] - a[2];
      b[6] = a[0] - a[6];
      
      a[1] = -dataptr[i*TEXTURE_BUFFER_STRIDE + 3] + dataptr[i*TEXTURE_BUFFER_STRIDE + 5] - dataptr[i*TEXTURE_BUFFER_STRIDE + 7] - (dataptr[i*TEXTURE_BUFFER_STRIDE + 7]>>1);
      a[3] =  dataptr[i*TEXTURE_BUFFER_STRIDE + 1] + dataptr[i*TEXTURE_BUFFER_STRIDE + 7] - dataptr[i*TEXTURE_BUFFER_STRIDE + 3] - (dataptr[i*TEXTURE_BUFFER_STRIDE + 3]>>1);
      a[5] = -dataptr[i*TEXTURE_BUFFER_STRIDE + 1] + dataptr[i*TEXTURE_BUFFER_STRIDE + 7] + dataptr[i*TEXTURE_BUFFER_STRIDE + 5] + (dataptr[i*TEXTURE_BUFFER_STRIDE + 5]>>1);
      a[7] =  dataptr[i*TEXTURE_BUFFER_STRIDE + 3] + dataptr[i*TEXTURE_BUFFER_STRIDE + 5] + dataptr[i*TEXTURE_BUFFER_STRIDE + 1] + (dataptr[i*TEXTURE_BUFFER_STRIDE + 1]>>1);
      
      b[1] = a[1] + (a[7]>>2);
      b[7] = -(a[1]>>2) + a[7];
      b[3] = a[3] + (a[5]>>2);
      b[5] = (a[3]>>2) - a[5];
      
      m6[0][i] = b[0] + b[7];
      m6[1][i] = b[2] + b[5];
      m6[2][i] = b[4] + b[3];
      m6[3][i] = b[6] + b[1];
      m6[4][i] = b[6] - b[1];
      m6[5][i] = b[4] - b[3];
      m6[6][i] = b[2] - b[5];
      m6[7][i] = b[0] - b[7];
    }
    
    // vertical inverse transform
    for( i=0; i<8; i++)
    {
      a[0] =  m6[i][0] + m6[i][4];
      a[4] =  m6[i][0] - m6[i][4];
      a[2] = (m6[i][2]>>1) - m6[i][6];
      a[6] =  m6[i][2] + (m6[i][6]>>1);
      
      b[0] = a[0] + a[6];
      b[2] = a[4] + a[2];
      b[4] = a[4] - a[2];
      b[6] = a[0] - a[6];
      
      a[1] = -m6[i][3] + m6[i][5] - m6[i][7] - (m6[i][7]>>1);
      a[3] =  m6[i][1] + m6[i][7] - m6[i][3] - (m6[i][3]>>1);
      a[5] = -m6[i][1] + m6[i][7] + m6[i][5] + (m6[i][5]>>1);
      a[7] =  m6[i][3] + m6[i][5] + m6[i][1] + (m6[i][1]>>1);
      
      b[1] =   a[1] + (a[7]>>2);
      b[7] = -(a[1]>>2) + a[7];
      b[3] =   a[3] + (a[5]>>2);
      b[5] =  (a[3]>>2) - a[5];
      
      dataptr[0*TEXTURE_BUFFER_STRIDE + i] = b[0] + b[7];
      dataptr[1*TEXTURE_BUFFER_STRIDE + i] = b[2] + b[5];
      dataptr[2*TEXTURE_BUFFER_STRIDE + i] = b[4] + b[3];
      dataptr[3*TEXTURE_BUFFER_STRIDE + i] = b[6] + b[1];
      dataptr[4*TEXTURE_BUFFER_STRIDE + i] = b[6] - b[1];
      dataptr[5*TEXTURE_BUFFER_STRIDE + i] = b[4] - b[3];
      dataptr[6*TEXTURE_BUFFER_STRIDE + i] = b[2] - b[5];
      dataptr[7*TEXTURE_BUFFER_STRIDE + i] = b[0] - b[7];
    }
}

/*
%macro SBUTTERFLY 4
    mova      m%4, m%2
    punpckl%1 m%2, m%3
    punpckh%1 m%4, m%3
    SWAP %3, %4
%endmacro

%macro TRANSPOSE8x8W 9-11
%ifdef ARCH_X86_64
    SBUTTERFLY wd,  %1, %2, %9
    SBUTTERFLY wd,  %3, %4, %9
    SBUTTERFLY wd,  %5, %6, %9
    SBUTTERFLY wd,  %7, %8, %9
    SBUTTERFLY dq,  %1, %3, %9
    SBUTTERFLY dq,  %2, %4, %9
    SBUTTERFLY dq,  %5, %7, %9
    SBUTTERFLY dq,  %6, %8, %9
    SBUTTERFLY qdq, %1, %5, %9
    SBUTTERFLY qdq, %2, %6, %9
    SBUTTERFLY qdq, %3, %7, %9
    SBUTTERFLY qdq, %4, %8, %9
    SWAP %2, %5
    SWAP %4, %7
%else
; in:  m0..m7, unless %11 in which case m6 is in %9
; out: m0..m7, unless %11 in which case m4 is in %10
; spills into %9 and %10
%if %0<11
    movdqa %9, m%7
%endif
    SBUTTERFLY wd,  %1, %2, %7
    movdqa %10, m%2
    movdqa m%7, %9
    SBUTTERFLY wd,  %3, %4, %2
    SBUTTERFLY wd,  %5, %6, %2
    SBUTTERFLY wd,  %7, %8, %2
    SBUTTERFLY dq,  %1, %3, %2
    movdqa %9, m%3
    movdqa m%2, %10
    SBUTTERFLY dq,  %2, %4, %3
    SBUTTERFLY dq,  %5, %7, %3
    SBUTTERFLY dq,  %6, %8, %3
    SBUTTERFLY qdq, %1, %5, %3
    SBUTTERFLY qdq, %2, %6, %3
    movdqa %10, m%2
    movdqa m%3, %9
    SBUTTERFLY qdq, %3, %7, %2
    SBUTTERFLY qdq, %4, %8, %2
    SWAP %2, %5
    SWAP %4, %7
%if %0<11
    movdqa m%5, %10
%endif
%endif
%endmacro 
*/

void TRANSPOSE_8x8( short *dataptr )
{
#ifndef X64_COMPATIBLE
	_asm{
		mov				esi,dataptr

		//transpose first two 4x4 blocks
		movdqa			xmm0, [esi]								//xmm0: 07 06 05 04 03 02 01 00
		movdqa			xmm1, [esi + 2*TEXTURE_BUFFER_STRIDE]	//xmm1: 17 16 15 14 13 12 11 10
		movdqa			xmm2, [esi + 4*TEXTURE_BUFFER_STRIDE]	//xmm2: 27 26 25 24 23 22 21 20
		movdqa			xmm3, [esi + 6*TEXTURE_BUFFER_STRIDE]	//xmm3: 37 36 35 34 33 32 31 30
		movdqa			xmm4, xmm0								//xmm4: 07 06 05 04 03 02 01 00
		movdqa			xmm5, xmm2								//xmm5: 27 26 25 24 23 22 21 20

		punpcklwd		xmm0, xmm1								//xmm0: 13 03 12 02 11 01 10 00
		punpcklwd		xmm2, xmm3								//xmm2: 33 23 32 22 31 21 30 20
		punpckhwd		xmm4, xmm1								//xmm4: 17 07 16 06 15 05 14 04
		punpckhwd		xmm5, xmm3								//xmm5: 37 27 36 26 35 25 34 24

		movdqa			xmm1, xmm0								//xmm1: 13 03 12 02 11 01 10 00
		movdqa			xmm3, xmm4								//xmm3: 17 07 16 06 15 05 14 04
		punpckldq		xmm0, xmm2								//xmm0: 31 21 11 01 30 20 10 00
		punpckldq		xmm4, xmm5								//xmm4: 35 25 15 05 34 24 14 04
		punpckhdq		xmm1, xmm2								//xmm1: 33 23 13 03 32 22 12 02
		punpckhdq		xmm3, xmm5								//xmm3: 37 27 17 07 36 26 16 06

		movdqa			xmm2, xmm4								//xmm2: 35 25 15 05 34 24 14 04
		pslldq			xmm2, 8									//xmm2: 34 24 14 04 xx xx xx xx
		movsd			xmm2, xmm0								//xmm2: 34 24 14 04 30 20 10 00		- line 0 DONE
		psrldq			xmm0, 8									//xmm0: xx xx xx xx 31 21 11 01
		movsd			xmm4, xmm0								//xmm4: 35 25 15 05 31 21 11 01		- line 1 DONE
		movdqa			xmm0, xmm3								//xmm0: 37 27 17 07 36 26 16 06
		pslldq			xmm0, 8									//xmm0: 36 26 16 06 xx xx xx xx
		movsd			xmm0, xmm1								//xmm0: 36 26 16 06 32 22 12 02		- line 2 DONE
		psrldq			xmm1, 8									//xmm1: xx xx xx xx 33 23 13 03
		movsd			xmm3, xmm1								//xmm3: 37 27 17 07 33 23 13 03		- line 3 DONE

		movdqa			[esi], xmm2
		movdqa			[esi + 2*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[esi + 4*TEXTURE_BUFFER_STRIDE], xmm0
		movdqa			[esi + 6*TEXTURE_BUFFER_STRIDE], xmm3
		//end

		//transpose the last two 4x4 blocks
		mov				esi,dataptr
		add				esi, 128
		movdqa			xmm0, [esi]								//xmm0: 07 06 05 04 03 02 01 00
		movdqa			xmm1, [esi + 2*TEXTURE_BUFFER_STRIDE]	//xmm1: 17 16 15 14 13 12 11 10
		movdqa			xmm2, [esi + 4*TEXTURE_BUFFER_STRIDE]	//xmm2: 27 26 25 24 23 22 21 20
		movdqa			xmm3, [esi + 6*TEXTURE_BUFFER_STRIDE]	//xmm3: 37 36 35 34 33 32 31 30
		movdqa			xmm4, xmm0								//xmm4: 07 06 05 04 03 02 01 00
		movdqa			xmm5, xmm2								//xmm5: 27 26 25 24 23 22 21 20

		punpcklwd		xmm0, xmm1								//xmm0: 13 03 12 02 11 01 10 00
		punpcklwd		xmm2, xmm3								//xmm2: 33 23 32 22 31 21 30 20
		punpckhwd		xmm4, xmm1								//xmm4: 17 07 16 06 15 05 14 04
		punpckhwd		xmm5, xmm3								//xmm5: 37 27 36 26 35 25 34 24

		movdqa			xmm1, xmm0								//xmm1: 13 03 12 02 11 01 10 00
		movdqa			xmm3, xmm4								//xmm3: 17 07 16 06 15 05 14 04
		punpckldq		xmm0, xmm2								//xmm0: 31 21 11 01 30 20 10 00
		punpckldq		xmm4, xmm5								//xmm4: 35 25 15 05 34 24 14 04
		punpckhdq		xmm1, xmm2								//xmm1: 33 23 13 03 32 22 12 02
		punpckhdq		xmm3, xmm5								//xmm3: 37 27 17 07 36 26 16 06

		movdqa			xmm2, xmm4								//xmm2: 35 25 15 05 34 24 14 04
		pslldq			xmm2, 8									//xmm2: 34 24 14 04 xx xx xx xx
		movsd			xmm2, xmm0								//xmm2: 34 24 14 04 30 20 10 00		- line 0 DONE
		psrldq			xmm0, 8									//xmm0: xx xx xx xx 31 21 11 01
		movsd			xmm4, xmm0								//xmm4: 35 25 15 05 31 21 11 01		- line 1 DONE
		movdqa			xmm0, xmm3								//xmm0: 37 27 17 07 36 26 16 06
		pslldq			xmm0, 8									//xmm0: 36 26 16 06 xx xx xx xx
		movsd			xmm0, xmm1								//xmm0: 36 26 16 06 32 22 12 02		- line 2 DONE
		psrldq			xmm1, 8									//xmm1: xx xx xx xx 33 23 13 03
		movsd			xmm3, xmm1								//xmm3: 37 27 17 07 33 23 13 03		- line 3 DONE

		movdqa			[esi], xmm2
		movdqa			[esi + 2*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[esi + 4*TEXTURE_BUFFER_STRIDE], xmm0
		movdqa			[esi + 6*TEXTURE_BUFFER_STRIDE], xmm3
		//end

		//Swrap top right and bottom left 4x4 blocks
		mov				esi,dataptr
		movdqa			xmm0, [esi]
		movdqa			xmm1, [esi + 2*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm2, [esi + 4*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm3, [esi + 6*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm4, [esi + 8*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm5, [esi + 10*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm6, [esi + 12*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm7, [esi + 14*TEXTURE_BUFFER_STRIDE]

		movlpd			[esi+8], xmm4
		movlpd			[esi+2*TEXTURE_BUFFER_STRIDE+8], xmm5
		movlpd			[esi+4*TEXTURE_BUFFER_STRIDE+8], xmm6
		movlpd			[esi+6*TEXTURE_BUFFER_STRIDE+8], xmm7

		movhpd			[esi + 8*TEXTURE_BUFFER_STRIDE], xmm0
		movhpd			[esi + 10*TEXTURE_BUFFER_STRIDE], xmm1
		movhpd			[esi + 12*TEXTURE_BUFFER_STRIDE], xmm2
		movhpd			[esi + 14*TEXTURE_BUFFER_STRIDE], xmm3

	}
#endif	
}/**/


void OneDimensionalInvDCT(short *dataptr)
{

#ifndef X64_COMPATIBLE

	__declspec(align(32)) short a0[8];
	__declspec(align(32)) short a1[8];
	__declspec(align(32)) short a2[8];
	__declspec(align(32)) short a3[8];
	__declspec(align(32)) short a4[8];
	__declspec(align(32)) short a5[8];
	__declspec(align(32)) short a6[8];
	__declspec(align(32)) short a7[8];
	__declspec(align(32)) short b0[8];
	__declspec(align(32)) short b1[8];
	__declspec(align(32)) short b2[8];
	__declspec(align(32)) short b3[8];
	__declspec(align(32)) short b4[8];
	__declspec(align(32)) short b5[8];
	__declspec(align(32)) short b6[8];
	__declspec(align(32)) short b7[8];

	_asm{

		mov			esi, dataptr

		movdqa			xmm0, [esi]
		movdqa			xmm2, [esi + 4*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm4, [esi + 8*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm6, [esi + 12*TEXTURE_BUFFER_STRIDE]

		movdqa			xmm1, xmm0
		movdqa			xmm3, xmm2
		movdqa			xmm5, xmm4
		movdqa			xmm7, xmm6

		paddsw			xmm0, xmm4
		psubsw			xmm1, xmm4
		psraw			xmm2, 1
		psubsw			xmm2, xmm6
		psraw			xmm6, 1
		paddsw			xmm3, xmm6
		movdqa			a0, xmm0
		movdqa			a4, xmm1
		movdqa			a2, xmm2
		movdqa			a6, xmm3								//a0: a[0]; a2: a[2]; a4: a[4]; a6: a[6]

		movdqa			xmm1, [esi + 2*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm3, [esi + 6*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm5, [esi + 10*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm7, [esi + 14*TEXTURE_BUFFER_STRIDE]

		movdqa			xmm0, xmm1
		movdqa			xmm2, xmm3
		movdqa			xmm4, xmm5
		movdqa			xmm6, xmm7

		psubsw			xmm5, xmm3
		psraw			xmm7, 1
		paddsw			xmm7, xmm6
		psubsw			xmm5, xmm7
		movdqa			a1, xmm5
		movdqa			xmm7, xmm6
		paddsw			xmm1, xmm7
		psraw			xmm3, 1
		paddsw			xmm3, xmm2
		psubsw			xmm1, xmm3
		movdqa			a3, xmm1
		movdqa			xmm1, xmm0
		movdqa			xmm5, xmm4
		movdqa			xmm7, xmm6
		psubsw			xmm7, xmm1
		psraw			xmm5, 1
		paddsw			xmm5, xmm4
		paddsw			xmm7, xmm5
		movdqa			a5, xmm7
		movdqa			xmm3, xmm2
		movdqa			xmm5, xmm4
		paddsw			xmm3, xmm5
		psraw			xmm1, 1
		paddsw			xmm1, xmm0
		paddsw			xmm1, xmm3
		movdqa			a7, xmm1										//a1: a[1]; a3: a[3]; a5: a[5]; a7: a[7]

		movdqa			xmm0, a0
		movdqa			xmm2, a2
		movdqa			xmm4, a4
		movdqa			xmm6, a6
		movdqa			xmm3, xmm2
		movdqa			xmm7, xmm6

		paddsw			xmm6, xmm0
		movdqa			b0, xmm6
		paddsw			xmm2, xmm4
		movdqa			b2, xmm2
		psubsw			xmm4, xmm3
		movdqa			b4, xmm4
		psubsw			xmm0, xmm7
		movdqa			b6, xmm0										//b0: b[0]; b2: b[2]; b4: b[4]; b6: b[6]

		movdqa			xmm1, a1
		movdqa			xmm3, a3
		movdqa			xmm5, a5
		movdqa			xmm7, a7
		movdqa			xmm4, xmm5
		movdqa			xmm6, xmm7

		psraw			xmm7, 2
		paddsw			xmm7, xmm1
		movdqa			b1, xmm7
		psraw			xmm1, 2
		psubsw			xmm6, xmm1
		movdqa			b7, xmm6
		psraw			xmm5, 2
		paddsw			xmm5, xmm3
		movdqa			b3, xmm5
		psraw			xmm3, 2
		psubsw			xmm3, xmm4
		movdqa			b5, xmm3										//b1: b[1]; b3: b[3]; b5: b[5]; b7: b[7]

		movdqa			xmm0, b0
		movdqa			xmm2, b2
		movdqa			xmm4, b4
		movdqa			xmm6, b6
		movdqa			xmm1, xmm0
		movdqa			xmm3, xmm2
		movdqa			xmm5, xmm4
		movdqa			xmm7, xmm6
		paddsw			xmm0, b7
		paddsw			xmm2, b5
		paddsw			xmm4, b3
		paddsw			xmm6, b1
		movdqa			[esi], xmm0
		movdqa			[esi + 2*TEXTURE_BUFFER_STRIDE], xmm2
		movdqa			[esi + 4*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[esi + 6*TEXTURE_BUFFER_STRIDE], xmm6
		movdqa			xmm0, xmm1
		movdqa			xmm2, xmm3
		movdqa			xmm4, xmm5
		movdqa			xmm6, xmm7
		psubsw			xmm6, b1
		psubsw			xmm4, b3
		psubsw			xmm2, b5
		psubsw			xmm0, b7
		movdqa			[esi + 8*TEXTURE_BUFFER_STRIDE], xmm6
		movdqa			[esi + 10*TEXTURE_BUFFER_STRIDE], xmm4
		movdqa			[esi + 12*TEXTURE_BUFFER_STRIDE], xmm2
		movdqa			[esi + 14*TEXTURE_BUFFER_STRIDE], xmm0

	}
#endif
}

void OneDimensionalDCT(short *dataptr,int short)
{
#ifndef X64_COMPATIBLE

	__declspec(align(32)) short a0[8];
	__declspec(align(32)) short a1[8];
	__declspec(align(32)) short a2[8];
	__declspec(align(32)) short a3[8];
	__declspec(align(32)) short b0[8];
	__declspec(align(32)) short b1[8];
	__declspec(align(32)) short b2[8];
	__declspec(align(32)) short b3[8];

	_asm{

		mov				esi, dataptr

		movdqa			xmm0, [esi]
		movdqa			xmm1, [esi + 2*TEXTURE_BUFFER_STRIDE]	
		movdqa			xmm2, [esi + 4*TEXTURE_BUFFER_STRIDE]
		movdqa			xmm3, [esi + 6*TEXTURE_BUFFER_STRIDE]

		movdqa			xmm4, xmm0
		movdqa			xmm5, xmm1
		movdqa			xmm6, xmm2
		movdqa			xmm7, xmm3

		paddsw			xmm0, [esi + 14*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm1, [esi + 12*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm2, [esi + 10*TEXTURE_BUFFER_STRIDE]
		paddsw			xmm3, [esi + 8*TEXTURE_BUFFER_STRIDE]

		psubsw			xmm4, [esi + 14*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm5, [esi + 12*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm6, [esi + 10*TEXTURE_BUFFER_STRIDE]
		psubsw			xmm7, [esi + 8*TEXTURE_BUFFER_STRIDE]			//xmm0-xmm7: a[0-7]

		movdqa			a0, xmm0
		movdqa			a1, xmm1
		movdqa			a2, xmm2
		movdqa			a3, xmm3
		paddsw			xmm0, xmm3
		paddsw			xmm1, xmm2
		movdqa			xmm2, a0
		movdqa			xmm3, a1
		psubsw			xmm2, a3
		psubsw			xmm3, a2										//xmm0-xmm3: b[0-3]; xmm4-xmm7: a[4-7]

		movdqa			a0, xmm4
		movdqa			a1, xmm5
		movdqa			a2, xmm6
		movdqa			a3, xmm7
		paddsw			xmm5, xmm6
		psraw			xmm4, 1
		paddsw			xmm4, a0
		paddsw			xmm5, xmm4										
		movdqa			b0, xmm5										//b0: b[4]
		
		movdqa			xmm4, a0
		psubsw			xmm4, xmm7
		psraw			xmm6, 1
		paddsw			xmm6, a2
		psubsw			xmm4, xmm6
		movdqa			b1, xmm4										//b1: b[5]

		movdqa			xmm4, a0
		movdqa			xmm5, a1
		paddsw			xmm4, xmm7
		psraw			xmm5, 1
		paddsw			xmm5, a1
		psubsw			xmm4, xmm5
		movdqa			b2, xmm4										//b2: b[6]

		movdqa			xmm5, a1
		movdqa			xmm6, a2
		psubsw			xmm5, xmm6
		psraw			xmm7, 1
		paddsw			xmm7, a3
		paddsw			xmm5, xmm7
		movdqa			b3, xmm5										//b3: b[7]

		movdqa			a0, xmm0
		movdqa			a1, xmm1
		movdqa			a2, xmm2
		movdqa			a3, xmm3										//a0-a3: b[0-3]

		paddsw			xmm0, a1
		movdqa			[esi], xmm0										//finish line0
		psraw			xmm3, 1
		paddsw			xmm3, xmm2
		movdqa			[esi + 4*TEXTURE_BUFFER_STRIDE], xmm3			//finish line2
		movdqa			xmm0, a0
		psubsw			xmm0, xmm1
		movdqa			[esi + 8*TEXTURE_BUFFER_STRIDE], xmm0			//finish line4
		movdqa			xmm3, a3
		psraw			xmm2, 1
		psubsw			xmm2, xmm3
		movdqa			[esi + 12*TEXTURE_BUFFER_STRIDE], xmm2			//finish line6

		movdqa			xmm4, b0
		movdqa			xmm7, b3
		psraw			xmm7, 2
		paddsw			xmm4, xmm7
		movdqa			[esi + 2*TEXTURE_BUFFER_STRIDE], xmm4			//finish line1
		movdqa			xmm5, b1
		movdqa			xmm6, b2
		psraw			xmm6, 2
		paddsw			xmm6, xmm5
		movdqa			[esi + 6*TEXTURE_BUFFER_STRIDE], xmm6			//finish line3
		movdqa			xmm6, b2
		psraw			xmm5, 2
		psubsw			xmm6, xmm5
		movdqa			[esi + 10*TEXTURE_BUFFER_STRIDE], xmm6			//finish line5
		movdqa			xmm4, b0
		movdqa			xmm7, b3
		psraw			xmm4, 2
		psubsw			xmm4, xmm7
		movdqa			[esi + 14*TEXTURE_BUFFER_STRIDE], xmm4			//finish line4

	}
#endif
}

void DoSpeedTestDCT(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	for( int loop=0;loop<LOOP_TEST_COUNT_LARGE;loop++)
	{
		memcpy( orio, address1, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE );
		memcpy( newo, address1, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE );
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int index = 0; index < MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE; index+=MB_SIZE*MB_SIZE )
			{
				SHORT *out =  (SHORT*)orio + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					Block_8x8_DCT( out );
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int index = 0; index < MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE; index+=MB_SIZE*MB_SIZE )
			{
				SHORT *out =  (SHORT*)newo + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					Block_8x8_DCT_Aligned_Stride16_YASM( out );
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("MS for old algorithm : %u\n",diffo);
	printf("for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	int ResultMatches = memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	printf("Results match : %s \n", ResultMatches == 0 ? "true":"false");
	if( ResultMatches != 0 )
	{
		for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE;i++)
			if( orio[i] != newo[i] )
				printf( "At %d, %d - %d - %d\n", i, orio[i], newo[i], address1[i] );
	}
}

void DoSpeedTestIDCT(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	for( int loop=0;loop<LOOP_TEST_COUNT_LARGE;loop++)
	{
		memcpy( address1, orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE );
		memcpy( address2, newo, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE );
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int index = 0; index < MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE; index+=MB_SIZE*MB_SIZE )
			{
				SHORT *out =  (SHORT*)address1 + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					Block_8x8_IDCT( out );
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int index = 0; index < MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE; index+=MB_SIZE*MB_SIZE )
			{
				SHORT *out =  (SHORT*)address2 + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					Block_8x8_IDCT_Aligned_Stride16_YASM( out );
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("MS for old algorithm : %u\n",diffo);
	printf("for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	int ResultMatches = memcmp( address1,address2, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE * sizeof( short ) );
	printf("Results match : %s \n", ResultMatches == 0 ? "true":"false");
	if( ResultMatches != 0 )
	{
		for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE - 4*MB_SIZE*MB_SIZE;i++)
			if( address1[i] != address2[i] )
				printf( "At %d, %d - %d - %d\n", i, address1[i], address2[i], newo[i] );
	}
}
void Transpose_8x8_C( short *input )
{
	short temp[TEXTURE_BUFFER_STRIDE][TEXTURE_BUFFER_STRIDE];
	int i,j;
	memcpy( temp, input, sizeof( temp ) );
	for( i=0; i<8; i++)
		for( j=0; j<8; j++)		
			input[TEXTURE_BUFFER_STRIDE*i+j]=temp[j][i];
}/**/

void DCT_8x8_1D_C( short *input )
{
	__declspec(align(16)) short a[8],b[8];
	for( int i=0; i<8; i++)
    {
      a[0] = input[i+0*TEXTURE_BUFFER_STRIDE] + input[i+7*TEXTURE_BUFFER_STRIDE];
      a[1] = input[i+1*TEXTURE_BUFFER_STRIDE] + input[i+6*TEXTURE_BUFFER_STRIDE];
      a[2] = input[i+2*TEXTURE_BUFFER_STRIDE] + input[i+5*TEXTURE_BUFFER_STRIDE];
      a[3] = input[i+3*TEXTURE_BUFFER_STRIDE] + input[i+4*TEXTURE_BUFFER_STRIDE];
      
      a[4] = input[i+0*TEXTURE_BUFFER_STRIDE] - input[i+7*TEXTURE_BUFFER_STRIDE];
      a[5] = input[i+1*TEXTURE_BUFFER_STRIDE] - input[i+6*TEXTURE_BUFFER_STRIDE];
      a[6] = input[i+2*TEXTURE_BUFFER_STRIDE] - input[i+5*TEXTURE_BUFFER_STRIDE];
      a[7] = input[i+3*TEXTURE_BUFFER_STRIDE] - input[i+4*TEXTURE_BUFFER_STRIDE];

	  b[0] = a[0] + a[3];
      b[1] = a[1] + a[2];
      b[2] = a[0] - a[3];
      b[3] = a[1] - a[2];    

      b[4]= a[5] + a[6] + ((a[4]>>1) + a[4]);
      b[5]= a[4] - a[7] - ((a[6]>>1) + a[6]);
      b[6]= a[4] + a[7] - ((a[5]>>1) + a[5]);
      b[7]= a[5] - a[6] + ((a[7]>>1) + a[7]);
      
	  input[i+0*TEXTURE_BUFFER_STRIDE] = b[0] + b[1];
	  input[i+1*TEXTURE_BUFFER_STRIDE] = b[4] + (b[7]>>2);
	  input[i+2*TEXTURE_BUFFER_STRIDE] = b[2] + (b[3]>>1);
	  input[i+3*TEXTURE_BUFFER_STRIDE] = b[5] + (b[6]>>2);
	  input[i+4*TEXTURE_BUFFER_STRIDE] = b[0] - b[1];
	  input[i+5*TEXTURE_BUFFER_STRIDE] = b[6] - (b[5]>>2);
	  input[i+6*TEXTURE_BUFFER_STRIDE] = (b[2]>>1) - b[3];
	  input[i+7*TEXTURE_BUFFER_STRIDE] = (b[4]>>2) - b[7];
/*
input[i+0*TEXTURE_BUFFER_STRIDE] = b[0];
input[i+1*TEXTURE_BUFFER_STRIDE] = b[1];
input[i+2*TEXTURE_BUFFER_STRIDE] = b[2];
input[i+3*TEXTURE_BUFFER_STRIDE] = b[3];
input[i+4*TEXTURE_BUFFER_STRIDE] = b[4];
input[i+5*TEXTURE_BUFFER_STRIDE] = b[5];
input[i+6*TEXTURE_BUFFER_STRIDE] = b[6];
input[i+7*TEXTURE_BUFFER_STRIDE] = b[7];
/*
input[i+0*TEXTURE_BUFFER_STRIDE] = a[0];
input[i+1*TEXTURE_BUFFER_STRIDE] = a[1];
input[i+2*TEXTURE_BUFFER_STRIDE] = a[2];
input[i+3*TEXTURE_BUFFER_STRIDE] = a[3];
input[i+4*TEXTURE_BUFFER_STRIDE] = a[4];
input[i+5*TEXTURE_BUFFER_STRIDE] = a[5];
input[i+6*TEXTURE_BUFFER_STRIDE] = a[6];
input[i+7*TEXTURE_BUFFER_STRIDE] = a[7];
*/
    }
}

void Block_8x8_DCT_ASM_C( short *input )
{
	Transpose_8x8_C( input );
	DCT_8x8_1D_C( input );
	Transpose_8x8_C( input );
	DCT_8x8_1D_C( input );
}

void Transpose_8x8_I( short *input )
{
	__m128i *p_input  = (__m128i*)input;
	__m128i *p_output = (__m128i*)input;
	__m128i a = _mm_load_si128(p_input);
	__m128i b = _mm_load_si128(p_input+2);
	__m128i c = _mm_load_si128(p_input+4);
	__m128i d = _mm_load_si128(p_input+6);
	__m128i e = _mm_load_si128(p_input+8);
	__m128i f = _mm_load_si128(p_input+10);
	__m128i g = _mm_load_si128(p_input+12);
	__m128i h = _mm_load_si128(p_input+14);

	__m128i a03b03 = _mm_unpacklo_epi16(a, b);
	__m128i a47b47 = _mm_unpackhi_epi16(a, b);
	__m128i c03d03 = _mm_unpacklo_epi16(c, d);
	__m128i c47d47 = _mm_unpackhi_epi16(c, d);

	__m128i a01b01c01d01 = _mm_unpacklo_epi32(a03b03, c03d03);
	__m128i a23b23c23d23 = _mm_unpackhi_epi32(a03b03, c03d03);
	__m128i a45b45c45d45 = _mm_unpacklo_epi32(a47b47, c47d47);
	__m128i a67b67c67d67 = _mm_unpackhi_epi32(a47b47, c47d47);

	__m128i e03f03 = _mm_unpacklo_epi16(e, f);
	__m128i e47f47 = _mm_unpackhi_epi16(e, f);
	__m128i g03h03 = _mm_unpacklo_epi16(g, h);
	__m128i g47h47 = _mm_unpackhi_epi16(g, h);

	__m128i e01f01g01h01 = _mm_unpacklo_epi32(e03f03, g03h03);
	__m128i e23f23g23h23 = _mm_unpackhi_epi32(e03f03, g03h03);
	__m128i e45f45g45h45 = _mm_unpacklo_epi32(e47f47, g47h47);
	__m128i e67f67g67h67 = _mm_unpackhi_epi32(e47f47, g47h47);

	__m128i a0b0c0d0e0f0g0h0 = _mm_unpacklo_epi64(a01b01c01d01, e01f01g01h01);
	__m128i a1b1c1d1e1f1g1h1 = _mm_unpackhi_epi64(a01b01c01d01, e01f01g01h01);
	__m128i a2b2c2d2e2f2g2h2 = _mm_unpacklo_epi64(a23b23c23d23, e23f23g23h23);
	__m128i a3b3c3d3e3f3g3h3 = _mm_unpackhi_epi64(a23b23c23d23, e23f23g23h23);
	__m128i a4b4c4d4e4f4g4h4 = _mm_unpacklo_epi64(a45b45c45d45, e45f45g45h45);
	__m128i a5b5c5d5e5f5g5h5 = _mm_unpackhi_epi64(a45b45c45d45, e45f45g45h45);
	__m128i a6b6c6d6e6f6g6h6 = _mm_unpacklo_epi64(a67b67c67d67, e67f67g67h67);
	__m128i a7b7c7d7e7f7g7h7 = _mm_unpackhi_epi64(a67b67c67d67, e67f67g67h67);

	_mm_store_si128(p_output+0, a0b0c0d0e0f0g0h0);
	_mm_store_si128(p_output+2, a1b1c1d1e1f1g1h1);
	_mm_store_si128(p_output+4, a2b2c2d2e2f2g2h2);
	_mm_store_si128(p_output+6, a3b3c3d3e3f3g3h3);
	_mm_store_si128(p_output+8, a4b4c4d4e4f4g4h4);
	_mm_store_si128(p_output+10, a5b5c5d5e5f5g5h5);
	_mm_store_si128(p_output+12, a6b6c6d6e6f6g6h6);
	_mm_store_si128(p_output+14, a7b7c7d7e7f7g7h7);
}

void IDCT_8x8_1D_C(short *input)
{
	//Copied from JM, to be optimized
	int m6[8][8];
	int a[8], b[8];
	int i;
	short *dataptr = input;

	for( i=0; i<8; i++)
    {
      a[0] = dataptr[i+0*TEXTURE_BUFFER_STRIDE] + dataptr[i+4*TEXTURE_BUFFER_STRIDE];
      a[4] = dataptr[i+0*TEXTURE_BUFFER_STRIDE] - dataptr[i+4*TEXTURE_BUFFER_STRIDE];
      a[2] = (dataptr[i+2*TEXTURE_BUFFER_STRIDE]>>1) - dataptr[i+6*TEXTURE_BUFFER_STRIDE];
      a[6] = dataptr[i+2*TEXTURE_BUFFER_STRIDE] + (dataptr[i+6*TEXTURE_BUFFER_STRIDE]>>1);
      
      b[0] = a[0] + a[6];
      b[2] = a[4] + a[2];
      b[4] = a[4] - a[2];
      b[6] = a[0] - a[6];
      
      a[1] = -dataptr[i+3*TEXTURE_BUFFER_STRIDE] + dataptr[i+5*TEXTURE_BUFFER_STRIDE] - dataptr[i+7*TEXTURE_BUFFER_STRIDE] - (dataptr[i+7*TEXTURE_BUFFER_STRIDE]>>1);
      a[3] =  dataptr[i+1*TEXTURE_BUFFER_STRIDE] + dataptr[i+7*TEXTURE_BUFFER_STRIDE] - dataptr[i+3*TEXTURE_BUFFER_STRIDE] - (dataptr[i+3*TEXTURE_BUFFER_STRIDE]>>1);
      a[5] = -dataptr[i+1*TEXTURE_BUFFER_STRIDE] + dataptr[i+7*TEXTURE_BUFFER_STRIDE] + dataptr[i+5*TEXTURE_BUFFER_STRIDE] + (dataptr[i+5*TEXTURE_BUFFER_STRIDE]>>1);
      a[7] =  dataptr[i+3*TEXTURE_BUFFER_STRIDE] + dataptr[i+5*TEXTURE_BUFFER_STRIDE] + dataptr[i+1*TEXTURE_BUFFER_STRIDE] + (dataptr[i+1*TEXTURE_BUFFER_STRIDE]>>1);
      
      b[1] = a[1] + (a[7]>>2);
      b[7] = -(a[1]>>2) + a[7];
      b[3] = a[3] + (a[5]>>2);
      b[5] = (a[3]>>2) - a[5];
      
  /*    dataptr[i+0*TEXTURE_BUFFER_STRIDE] = a[0];
      dataptr[i+1*TEXTURE_BUFFER_STRIDE] = a[1];
      dataptr[i+2*TEXTURE_BUFFER_STRIDE] = a[2];
      dataptr[i+3*TEXTURE_BUFFER_STRIDE] = a[3];
      dataptr[i+4*TEXTURE_BUFFER_STRIDE] = a[4];
      dataptr[i+5*TEXTURE_BUFFER_STRIDE] = a[5];
      dataptr[i+6*TEXTURE_BUFFER_STRIDE] = a[6];
      dataptr[i+7*TEXTURE_BUFFER_STRIDE] = a[7];/**/
/*      dataptr[i+0*TEXTURE_BUFFER_STRIDE] = b[0];
      dataptr[i+1*TEXTURE_BUFFER_STRIDE] = b[1];
      dataptr[i+2*TEXTURE_BUFFER_STRIDE] = b[2];
      dataptr[i+3*TEXTURE_BUFFER_STRIDE] = b[3];
      dataptr[i+4*TEXTURE_BUFFER_STRIDE] = b[4];
      dataptr[i+5*TEXTURE_BUFFER_STRIDE] = b[5];
      dataptr[i+6*TEXTURE_BUFFER_STRIDE] = b[6];
      dataptr[i+7*TEXTURE_BUFFER_STRIDE] = b[7];/**/
      dataptr[i+0*TEXTURE_BUFFER_STRIDE] = b[0] + b[7];
      dataptr[i+1*TEXTURE_BUFFER_STRIDE] = b[2] + b[5];
      dataptr[i+2*TEXTURE_BUFFER_STRIDE] = b[4] + b[3];
      dataptr[i+3*TEXTURE_BUFFER_STRIDE] = b[6] + b[1];
      dataptr[i+4*TEXTURE_BUFFER_STRIDE] = b[6] - b[1];
      dataptr[i+5*TEXTURE_BUFFER_STRIDE] = b[4] - b[3];
      dataptr[i+6*TEXTURE_BUFFER_STRIDE] = b[2] - b[5];
      dataptr[i+7*TEXTURE_BUFFER_STRIDE] = b[0] - b[7];/**/
    }
}

void Block_8x8_IDCT_ASM_C( short *input )
{
	Transpose_8x8_C( input );
	IDCT_8x8_1D_C( input );
	Transpose_8x8_C( input );
	IDCT_8x8_1D_C( input );
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE *taddress1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32);
	BYTE *taddress2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
	UINT* torio = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
	UINT* tnewo = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
	BYTE* address1;
	BYTE* address2;
	UINT* orio;
	UINT* newo;

	//break alignment to get worst case scenario
	address1 = AllignAddress( taddress1 );
	address2 = AllignAddress( taddress2 );
	orio = AllignAddress( torio );
	newo = AllignAddress( tnewo );

	memset( orio, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( newo, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE;i++)
	{
		((short*)address1)[i] = i % 255;
		((short*)address2)[i] = i % 255;
	}
	//step by step debugging an development issue
/*	{
		//Block_8x8_DCT( (short*)address1 );
		__declspec(align(16)) short temp1[16*16];
		__declspec(align(16)) short temp2[16*16];
		__declspec(align(16)) short temp3[16*16];
		for(int i=0;i<16*16;i++)
		{
			temp1[i]=i;
			temp2[i]=i;
			temp3[i]=i;
		}


		//test transpose
//#define TESTING_TRANSPOSE
#ifdef TESTING_TRANSPOSE
		Transpose_8x8_C( (short*)temp1 );
		//Transpose_8x8_C( (short*)temp1 );
		Transpose_8x8_I( (short*)temp2 );
		Transpose_8x8_I( (short*)temp2 );
		//Transpose_8x8_I( (short*)temp2 );
		Block_8x8_DCT_Aligned_Stride16_YASM( (short*)temp3 );
		Block_8x8_DCT_Aligned_Stride16_YASM( (short*)temp3 );
		if( memcmp( temp2, temp3, sizeof( temp2 ) ) == 0 )
			printf("Happy panda\n");
		else
		{
			for(int i=0;i<16*16;i++)
				if( temp2[i] != temp3[i] )
					printf("One\n");
		}
#endif

		//test dct
//#define TESTING_PARTIAL_DCT
#ifdef TESTING_PARTIAL_DCT
		Block_8x8_DCT_ASM_C( temp1 );
//		Block_8x8_DCT( temp2 );
//		if( memcmp( temp1, temp2, sizeof( temp1 ) ) != 0 )
//			printf("Sad panda 1\n");
		Block_8x8_DCT_Aligned_Stride16_YASM( (short*)temp3 );
		if( memcmp( temp1, temp3, sizeof( temp3 ) ) != 0 )
		{
			printf("Sad panda 2\n");
			for(int i=0;i<16*16;i++)
				if( temp1[i] != temp3[i] )
					printf("One\n");
		}
		Block_8x8_DCT_ASM_C( (short*)address1 );
		Block_8x8_DCT_Aligned_Stride16_YASM( (short*)address2 );
//		Block_8x8_DCT( (short*)address2 );
		if( memcmp( address1, address2, sizeof( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE ) ) != 0 )
		{
			printf("Sad panda 2\n");
			for(int i=0;i<16*16;i++)
				if( ((short*)address1)[i] != ((short*)address2)[i] )
					printf("One\n");
		}
#endif
#define TESTING_PARTIAL_IDCT
#ifdef TESTING_PARTIAL_IDCT
		//Block_8x8_IDCT( temp1 );
		Block_8x8_IDCT_ASM_C( temp2 );
		if( memcmp( temp1, temp2, sizeof( temp1 ) ) != 0 )
			printf("Sad panda 1\n");
		Block_8x8_IDCT_Aligned_Stride16_YASM( (short*)temp3 );
		if( memcmp( temp2, temp3, sizeof( temp3 ) ) != 0 )
		{
			printf("Sad panda 2\n");
			for(int i=0;i<16*16;i++)
				if( temp2[i] != temp3[i] )
					printf("One\n");
		}
#endif
	}/**/

//do speed test
	DoSpeedTestDCT( address1, stride, address2, stride, orio, newo );
	DoSpeedTestIDCT( address1, stride, address2, stride, orio, newo );

	getch();
	free( taddress1 );
	free( taddress2 );
	free( torio );
	free( tnewo );

	return 0;

}