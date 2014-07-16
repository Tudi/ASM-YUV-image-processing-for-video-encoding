#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>
//#include <tmmintrin.h>
//#include "tmmintrin.h"

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT	1	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT	20	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	600	//to have a larger runtime
#endif

INT ALPHA_TABLE[52]  = {0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,4,4,5,6,  7,8,9,10,12,13,15,17,  20,22,25,28,32,36,40,45,  50,56,63,71,80,90,101,113,  127,144,162,182,203,226,255,255} ;
INT BETA_TABLE[52]  = {0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,2,2,2,3,  3,3,3, 4, 4, 4, 6, 6,   7, 7, 8, 8, 9, 9,10,10,  11,11,12,12,13,13, 14, 14,   15, 15, 16, 16, 17, 17, 18, 18} ;
INT CLIP_TABLE[52][3] =
{
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},
	{0,0,0},	{0,0,1},	{0,0,1},	{0,0,1},
	{0,0,1},	{0,1,1},	{0,1,1},	{1,1,1},
	{1,1,1},	{1,1,1},	{1,1,1},	{1,1,2},
	{1,1,2},	{1,1,2},	{1,1,2},	{1,2,3},
	{1,2,3},	{2,2,3},	{2,2,4},	{2,3,4},
	{2,3,4},	{3,3,5},	{3,4,6},	{3,4,6},
	{4,5,7},	{4,5,8},	{4,6,9},	{5,7,10},
	{6,8,11},	{6,8,13},	{7,10,14},	{8,11,16},
	{9,12,18},	{10,13,20},	{11,15,23},	{13,17,25}
};

// 4 blocks stripe: order is 0 1 2 3 - for both directions
UINT INTRA_STRENGTH[4] = { 0x04040404, 0x03030303, 0x03030303, 0x03030303 };

//
//	NOTE:
//		Indexes are in raster scan order
//		because cbp_blks[] is filled this way by CTextureEncoder::GetCBP
//
INT BLK_NUM[2][4][4]  =
{
	{	// VERTICAL EDGE
		{ 0, 2, 8, 10 },
		{ 1, 3, 9, 11 },
		{ 4, 6, 12, 14 },
		{ 5, 7, 13, 15 }
	},
	{	// HORIZONTAL EDGE
		{ 0, 1, 4, 5 },
		{ 2, 3, 6, 7 },
		{ 8, 9, 12, 13 },
		{ 10, 11, 14, 15 },
	}
};

//! make chroma QP from quant
const BYTE QP_SCALE_CR[52]=
{
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
   28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,37,38,38,38,39,39,39,39
};

#define ABS(x)			( (x) >= 0 ? (x) : (-(x)) )
#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))

#define P0_H(stride)	P[2*stride + stride]
#define P1_H(stride)	P[2*stride]
#define P2_H(stride)	P[stride]
//#define P3_H(stride)	P[0]
#define Q0_H(stride)	Q[0]
#define Q1_H(stride)	Q[stride]
#define Q2_H(stride)	Q[2*stride]
//#define Q3_H(stride)	Q[2*stride + stride]

#define T_IDX_A 32
#define T_IDX_B 32
#define T_BS	T_IDX_A

void FilterHorBS_123( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
	INT line;
	INT delta_p0q0, C0, C, DELTA;
	INT ap = 0, aq = 0;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	bs--;				// table is from 0..2

	C0 = m_C0_TAB[bs];

	for( line = 0; line < 4; line++ )
	{
		// current line
		INT delta_p0q0 = ABS( P0_H(stride) - Q0_H(stride) );
		INT delta_p1p0 = ABS( P1_H(stride) - P0_H(stride) );
		INT delta_q1q0 = ABS( Q1_H(stride) - Q0_H(stride) );

		if( (delta_p0q0 < m_alpha) &&
			(delta_p1p0 < m_beta) &&
			(delta_q1q0 < m_beta) )
		{
			const INT pp0 = P0_H(stride);
			const INT qq0 = Q0_H(stride);

			ap = (ABS( P2_H(stride) - P0_H(stride) ) - m_beta) < 0;
			aq = (ABS( Q2_H(stride) - Q0_H(stride) ) - m_beta) < 0;

			C = C0 + ap + aq;

			INT Q0P04 = Q0_H(stride) - P0_H(stride);
			Q0P04 = Q0P04 << 2;
			INT P1Q14=P1_H(stride) - Q1_H(stride);
			P1Q14 += 4;
			DELTA = Q0P04 + P1Q14;
			DELTA = DELTA >> 3;
			DELTA = CLIP3( -C, C, ( (((Q0_H(stride) - P0_H(stride))<<2) + (P1_H(stride) - Q1_H(stride)) + 4) >> 3) );

			P0_H(stride) = CLIP1( (P0_H(stride) + DELTA) );
			Q0_H(stride) = CLIP1( (Q0_H(stride) - DELTA) );

			if( ap )
				P1_H(stride) = P1_H(stride) + CLIP3( -C0, C0, ( (P2_H(stride) + ((pp0+qq0+1)>>1) - (P1_H(stride)<<1)) >> 1 ) );

			if( aq )
				Q1_H(stride) = Q1_H(stride) + CLIP3( -C0, C0, ( (Q2_H(stride) + ((pp0+qq0+1)>>1) - (Q1_H(stride)<<1)) >> 1 ) );
		}

		P++;
		Q++;
	}
}

void FilterHorBS_123_asm( BYTE *Q, BYTE *P, INT blk, INT bs, int m_rec_luma_stride )
{
//	INT line;
	INT  C0;
//	BYTE *PixBuff;
	INT i;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	blk <<= 5;
	bs--;				// table is from 0..2

	C0 = m_C0_TAB[bs];

	INT beta=m_beta;
	INT alpha=m_alpha;
	INT rec_stride=m_rec_luma_stride;

	__declspec(align(32)) static SHORT ONES_MASK[4]={1,1,1,1};
	__declspec(align(32)) static SHORT FOUR_MASK[4]={4,4,4,4};
	__declspec(align(32)) static SHORT MASK_32767[4]={32767,32767,32767,32767};


/*
	SHORT TP0[4];
	SHORT TP1[4];
	SHORT TP2[4];

	SHORT TQ0[4];
	SHORT TQ1[4];
	SHORT TQ2[4];
*/

	__declspec(align(32)) SHORT TALPHA[4];
	__declspec(align(32)) SHORT TBETA[4];
	__declspec(align(32)) SHORT CTAB[4];
	__declspec(align(32)) SHORT TPQ[6*4];

	__declspec(align(32)) SHORT COND_TAB[4];
//	SHORT AP_TAB[4];
//	SHORT AQ_TAB[4]


	//TTT
/*
	SHORT D_p0_q0[4];
	SHORT D_p1_p0[4];
	SHORT D_q1_q0[4];

	SHORT RES[4];
*/

	for(i=0;i<4;i++)
	{
		// current line
		//	PixBuff = &m_EdgePixBuff[line];

		// current line
/*		PixBuff = &m_EdgePixBuff[blk+8*i];

		TP0[i]=p0;
		TP1[i]=p1;
		TP2[i]=p2;
		TQ0[i]=q0;
		TQ1[i]=q1;
		TQ2[i]=q2;
*/
		TALPHA[i]=m_alpha;
		TBETA[i]=m_beta;
		CTAB[i]=C0; //set to C0, then add ap and aq

	}

#define TP0 [TPQ+0]
#define TP1 [TPQ+8]
#define TP2 [TPQ+16]

#define TQ0 [TPQ+24]
#define TQ1 [TPQ+32]
#define TQ2 [TPQ+40]
		//MAKE TPQ TAB
	__asm
	{
		mov esi, P
		mov eax,rec_stride
		add esi, eax
		//stride is added at call
		//lea esi, [esi+eax]
		lea ecx, [eax+2*eax]

		pxor mm6, mm6
		pxor mm7, mm7

		movd mm0, [esi]
		movd mm1, [esi+eax]
		movd mm2, [esi+2*eax]
		movd mm3, [esi+ecx]

		lea esi,[esi+4*eax]
		movd mm4, [esi]
		movd mm5, [esi+eax]

		punpcklbw mm0,mm6
		punpcklbw mm1,mm7
		punpcklbw mm2,mm6
		punpcklbw mm3,mm7
		punpcklbw mm4,mm6
		punpcklbw mm5,mm7


		//p2 is m_EdgePixBuff[1]
		movq TP2,mm0

		//p1 is m_EdgePixBuff[2]
		movq TP1,mm1
		//p0 is m_EdgePixBuff[3]
		movq TP0, mm2

		//q0 is m_EdgePixBuff[4]
		movq TQ0,mm3

		//q1 is m_EdgePixBuff[5]
		movq TQ1,mm4

		//q2 is m_EdgePixBuff[6]
		movq TQ2, mm5
	}


	__asm
	{
		mov esi,P
		mov edi, Q
		mov eax, rec_stride
		lea ecx,[eax+2*eax]

		//p0
		movq mm0, TP0
		//p1
		movq mm1, TP1
		//q0
		movq mm2, TQ0
		//q1
		movq mm3, TQ1

		//p0
		movq mm4, mm0

		//q1-q0
		psubw mm3, mm2
		//p0-q0
		psubw mm0, mm2

		//p1-p0
		psubw mm1, mm4


		//abs values
		movq mm2, mm3
		movq mm4, mm0
		movq mm5, mm1
		//sign ext
		psraw mm3, 15
		psraw mm0, 15
		psraw mm1, 15

		pxor  mm2,mm3
		pxor  mm4,mm0
		pxor  mm5,mm1

		psubw mm2,mm3
		psubw mm4,mm0
		psubw mm5,mm1


		//sub values
		psubw mm2,TBETA
		psubw mm5,TBETA
		psubw mm4,TALPHA

		pand mm2,mm5
		pand mm2,mm4
		psraw mm2,15

		//RESULT FOR CONDITION
		movq COND_TAB, mm2

		//check if condition is false for all pixels
		packsswb mm2,mm2
		movd edx,mm2
		cmp edx, 0
		je _over

		//AP and AQ
		movq mm0, TP2
		movq mm1, TQ2
		//delta_p2_p0
		psubw mm0, TP0
		//delta_q2_q0
		psubw mm1, TQ0
		//save
		movq  mm2, mm0
		movq  mm3, mm1
		//sign
		psraw mm0, 15
		psraw mm1, 15
		//
		pxor mm2, mm0
		pxor mm3, mm1
		//pabs
		psubw mm2,mm0
		psubw mm3,mm1

		//beta
		psubw mm2, TBETA
		psubw mm3, TBETA

		//RESULT FOR AP AND AQ
		// mm2 and mm3 must be not modified until _P0_H_Q0_H
		//ap and aq are 0 or -1
		psraw mm2, 15
		psraw mm3, 15


		//check ap and skip p1_h if 0
		movq mm0, mm2
		packsswb mm0,mm0
		movd edx, mm0
		cmp edx, 0
		je _Q1_H

		//make P1_H

		movq mm0, TP0
		movq mm4, TQ0
		movq mm7, TP1
		movq mm6, TP2
		//save p1
		movq mm5, mm7
		//p0+q0
		paddw mm0, mm4
		// (p1<<1)
		psllw mm7,1
		//p0+q0+1
		paddw mm0, ONES_MASK
		//p2 - (p1<<1)
		psubw mm6,mm7
		//(p0+q0+1)>>1
		psraw mm0,1

		//( (p2 + ((p0+q0+1)>>1) - (p1<<1)) >> 1 )
		paddw mm6,mm0
		psraw mm6,1

		movq mm0, MASK_32767
		psubw mm0, CTAB //ctab is C0


	//  CLIP to C0
		psubsw mm6, mm0
		psubsw mm6, ONES_MASK

		paddw mm6, mm0
		paddw mm6, ONES_MASK

		paddsw mm6, mm0
		psubw mm6,  mm0

		//add p1 ---->p1+CLIP3( -C0, C0, ( (p2 + ((p0+q0+1)>>1) - (p1<<1)) >> 1 ) );
		paddw mm6,mm5

		//get ap mask
		movq mm4, mm2
		//and with cond mask
		pand mm4, COND_TAB

		//keep only coeff for which ap is not 0 and cond is not 0
		pand mm6, mm4


		//reset mm7
		pxor mm7,mm7

		//LOAD P1_H
		//#define P1_H(stride)	P[2*stride]
		movd mm0,[esi+2*eax]
		//expand to short
		punpcklbw mm0,mm7

		pandn mm4, mm0 //mm4 is (ap&cond) mask, invertd to reset source

		//or source with result
		por mm4, mm6

		packuswb mm4,mm7

		// RESULT FOR P1_H
		movd [esi+2*eax],mm4

_Q1_H:
		//check aq and skip q1_h if 0
		movq mm0, mm3
		packsswb mm0,mm0
		movd edx, mm0
		cmp edx, 0
		je _P0_H_Q0_H

		//make Q1_H
		movq mm0, TP0
		movq mm4, TQ0
		movq mm7, TQ1
		movq mm6, TQ2
		//save q1
		movq mm5, mm7
		//p0+q0
		paddw mm0, mm4
		// (q1<<1)
		psllw mm7,1
		//p0+q0+1
		paddw mm0, ONES_MASK
		//q2 - (q1<<1)
		psubw mm6,mm7
		//(p0+q0+1)>>1
		psraw mm0,1

		//( (q2 + ((p0+q0+1)>>1) - (q1<<1)) >> 1 )
		paddw mm6,mm0
		psraw mm6,1

		movq mm0, MASK_32767
		psubw mm0, CTAB //ctab is C0


	//  CLIP to C0
		psubsw mm6, mm0
		psubsw mm6, ONES_MASK

		paddw mm6, mm0
		paddw mm6, ONES_MASK

		paddsw mm6, mm0
		psubw mm6,  mm0

		//add q1 ---->q1+CLIP3( -C0, C0, ( (q2 + ((p0+q0+1)>>1) - (q1<<1)) >> 1 ) );
		paddw mm6,mm5


		//get aq mask
		movq  mm4, mm3
		//and with cond mask
		pand mm4, COND_TAB

		//keep only coeffs with aq and cond not 0
		pand mm6, mm4

		//reset mm7
		pxor mm7,mm7

		//LOAD Q1_H
		//#define Q1_H(stride)	Q[1*stride]
		movd mm0,[edi+eax]
		//expand to short
		punpcklbw mm0,mm7

		pandn mm4, mm0 //mm4 is (aq&cond) mask, invertd to reset source

		//or source with result
		por mm4, mm6

		packuswb mm4,mm7

		movd [edi+eax],mm4

_P0_H_Q0_H:
		//make C =C0-ap-aq (because ap=-1, aq=-1)
		movq mm1,CTAB
		psubw mm1 , mm2
		psubw mm1, mm3
		movq mm0, MASK_32767
		psubw mm0,mm1 //mm0 is 32767-C

		movq CTAB,mm1

		// COMNPUTE DELTA
		//	DELTA = CLIP3( -C, C, ( (((q0 - p0)<<2) + (p1 - q1) + 4) >> 3) );
		//p0
		movq mm1, TP0
		//p1
		movq mm2, TP1
		//q0
		movq mm3, TQ0
		//q1
		movq mm4, TQ1

		//q0-p0
		psubw mm3, mm1
		//p1-q1
		psubw mm2, mm4

		psllw mm3, 2

		paddw mm2, FOUR_MASK

		paddw mm2,mm3

		psraw mm2,3

		//CLIP to C
		psubsw mm2, mm0
		psubsw mm2, ONES_MASK

		paddw mm2, mm0
		paddw mm2, ONES_MASK

		paddsw mm2, mm0
		psubw mm2,  mm0

		//TEST
		//	movq RES,mm2


		// P0_H(rec_stride) = CLIP1( (p0 + DELTA) );
		//	Q0_H(rec_stride) = CLIP1( (q0 - DELTA) );

		movq mm4, TP0
		movq mm5, TQ0

		paddw mm4, mm2
		psubw mm5, mm2

//		movq RES, mm4
//		movq RES, mm5

		pxor mm7,mm7
		// P0_H(stride)	P[3*stride]
		movd mm0, [esi+ecx]
		// Q0_H(stride)	Q[0*stride]
		movd mm1, [edi]

		punpcklbw mm0,mm7
		punpcklbw mm1,mm7

		movq mm2,COND_TAB
		movq mm3,COND_TAB

		//set result with condition
		pand mm4,mm2
		pand mm5,mm3

		//keep only source with no cond

		pandn mm2, mm0
		pandn mm3, mm1

		por  mm2, mm4
		por  mm3, mm5

		packuswb mm2,mm7
		packuswb mm3,mm7

		// P0_H(stride)	P[3*stride]
		movd [esi+ecx],mm2
		// Q0_H(stride)	Q[0*stride]
		movd [edi],mm3

_over:

		emms
	}

#undef TP0
#undef TP1
#undef TP2

#undef TQ0
#undef TQ1
#undef TQ2
}

void FilterHorBS_123_intr1( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_1[8],test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_srai_epi16( delta_p0q0, 15 );
		adelta_p1p0 = _mm_srai_epi16( delta_p1p0, 15 );
		adelta_q1q0 = _mm_srai_epi16( delta_q1q0, 15 );
		delta_p0q0 = _mm_xor_si128( adelta_p0q0, delta_p0q0 );
		delta_p1p0 = _mm_xor_si128( adelta_p1p0, delta_p1p0 );
		delta_q1q0 = _mm_xor_si128( adelta_q1q0, delta_q1q0 );
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, adelta_p0q0 );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, adelta_p1p0 );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, adelta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		__declspec(align(16)) __m128i t_cond;
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		t_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		t_cond = _mm_and_si128( t_cond, delta_q1q0 );
		_mm_storel_epi64((__m128i*)test_cond_1,t_cond);
	}/**/
//	if( test_cond_1[0] || test_cond_1[1] || test_cond_1[2] || test_cond_1[3] ) //causes almost 20% speed drop
	{
		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta,adelta_p2p0beta,adelta_q2q0beta;
		delta_p2p0beta = _mm_sub_epi16(p2_r1, p0_r3);
		adelta_p2p0beta = _mm_srai_epi16( delta_p2p0beta, 15 );
		delta_p2p0beta = _mm_xor_si128( adelta_p2p0beta, delta_p2p0beta );
		delta_p2p0beta = _mm_sub_epi16( delta_p2p0beta, adelta_p2p0beta );
		delta_p2p0beta = _mm_sub_epi16( delta_p2p0beta, m_beta_vect );
		delta_p2p0beta = _mm_srli_epi16( delta_p2p0beta, 15 );	//if this is 1 then we will store result
		_mm_storel_epi64((__m128i*)test_cond_4,delta_p2p0beta);

		delta_q2q0beta = _mm_sub_epi16(q2_r2, q0_r0);
		adelta_q2q0beta = _mm_srai_epi16( delta_q2q0beta, 15 );
		delta_q2q0beta = _mm_xor_si128( adelta_q2q0beta, delta_q2q0beta );
		delta_q2q0beta = _mm_sub_epi16( delta_q2q0beta, adelta_q2q0beta );
		delta_q2q0beta = _mm_sub_epi16( delta_q2q0beta, m_beta_vect );
		delta_q2q0beta = _mm_srli_epi16(delta_q2q0beta, 15);	//if this is 1 then we will store result
		_mm_storel_epi64((__m128i*)test_cond_5,delta_q2q0beta);

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_p0 = _mm_max_epi16( res_p0 , zero );		//clip1
		res_p0 = _mm_packus_epi16( res_p0, res_p0 );	//clip1
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		res_q0 = _mm_max_epi16( res_q0 , zero );		//clip1
		res_q0 = _mm_packus_epi16( res_q0, res_q0 );	//clip1
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < 0 )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_4[i] )
					P1_H(stride) = res_P_1[i];
				if( test_cond_5[i] )
					Q1_H(stride) = res_Q_1[i];
			}
			P++;
			Q++;
		}
	}
/*	for( line = 0; line < 4; line++ )
	{
		// current line
		PixBuff = P;

		if( (ABS(P0_H(stride) - Q0_H(stride)) < m_alpha) &&
			(ABS(P1_H(stride) - P0_H(stride)) < m_beta) &&
			(ABS(Q1_H(stride) - Q0_H(stride)) < m_beta) )
		{
			const INT pp0 = P0_H(stride);
			const INT qq0 = Q0_H(stride);

			ap = (ABS( P2_H(stride) - P0_H(stride) ) - m_beta) < 0;
			aq = (ABS( Q2_H(stride) - Q0_H(stride) ) - m_beta) < 0;

			C = C0 + ap + aq;

			DELTA = CLIP3( -C, C, ( (((Q0_H(stride) - P0_H(stride))<<2) + (P1_H(stride) - Q1_H(stride)) + 4) >> 3) );

			P0_H(stride) = CLIP1( (P0_H(stride) + DELTA) );
			Q0_H(stride) = CLIP1( (Q0_H(stride) - DELTA) );

			if( ap )
				P1_H(stride) = P1_H(stride) + CLIP3( -C0, C0, ( (P2_H(stride) + ((pp0+qq0+1)>>1) - (P1_H(stride)<<1)) >> 1 ) );

			if( aq )
				Q1_H(stride) = Q1_H(stride) + CLIP3( -C0, C0, ( (Q2_H(stride) + ((pp0+qq0+1)>>1) - (Q1_H(stride)<<1)) >> 1 ) );
		}

		P++;
		Q++;
	} */
}

void FilterHorBS_123_intr2( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_1[8],test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_srai_epi16( delta_p0q0, 15 );
		adelta_p1p0 = _mm_srai_epi16( delta_p1p0, 15 );
		adelta_q1q0 = _mm_srai_epi16( delta_q1q0, 15 );
		delta_p0q0 = _mm_xor_si128( adelta_p0q0, delta_p0q0 );
		delta_p1p0 = _mm_xor_si128( adelta_p1p0, delta_p1p0 );
		delta_q1q0 = _mm_xor_si128( adelta_q1q0, delta_q1q0 );
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, adelta_p0q0 );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, adelta_p1p0 );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, adelta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		__declspec(align(16)) __m128i t_cond;
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		t_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		t_cond = _mm_and_si128( t_cond, delta_q1q0 );
		_mm_storel_epi64((__m128i*)test_cond_1,t_cond);
	}/**/
	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_p0 = _mm_max_epi16( res_p0 , zero );		//clip1
		res_p0 = _mm_packus_epi16( res_p0, res_p0 );	//clip1
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		res_q0 = _mm_max_epi16( res_q0 , zero );		//clip1
		res_q0 = _mm_packus_epi16( res_q0, res_q0 );	//clip1
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < 0 )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_45[i] )
					P1_H(stride) = res_P_1[i];
				if( test_cond_45[i+4] )
					Q1_H(stride) = res_Q_1[i];
			}
			P++;
			Q++;
		}
	}
}

void FilterHorBS_123_intr3( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_1[8],test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) short res_P_0[8],res_Q_0[8];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];
	__declspec(align(16)) short res_P1_Q1[16];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_srai_epi16( delta_p0q0, 15 );
		adelta_p1p0 = _mm_srai_epi16( delta_p1p0, 15 );
		adelta_q1q0 = _mm_srai_epi16( delta_q1q0, 15 );
		delta_p0q0 = _mm_xor_si128( adelta_p0q0, delta_p0q0 );
		delta_p1p0 = _mm_xor_si128( adelta_p1p0, delta_p1p0 );
		delta_q1q0 = _mm_xor_si128( adelta_q1q0, delta_q1q0 );
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, adelta_p0q0 );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, adelta_p1p0 );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, adelta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		__declspec(align(16)) __m128i t_cond;
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		t_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		t_cond = _mm_and_si128( t_cond, delta_q1q0 );
		_mm_storel_epi64((__m128i*)test_cond_1,t_cond);
	}/**/
	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep0q0one,p2_r1q2_r2,p1_r2q1_r1,p1sq1s,res;
		p1_r2q1_r1 = _mm_unpacklo_epi64(p1_r2, q1_r1);
		p0q0onep0q0one = _mm_unpacklo_epi64(p0q0one, p0q0one);
		p2_r1q2_r2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p1sq1s = _mm_slli_epi16(p1_r2q1_r1, 1);

		res = _mm_adds_epi16( p0q0onep0q0one,p2_r1q2_r2 );
		res = _mm_sub_epi16( res,p1sq1s );
		res = _mm_srai_epi16( res, 1);
		res = _mm_min_epi16( res , high_limit );
		res = _mm_max_epi16( res , low_limit );
		res = _mm_adds_epi16( res, p1_r2q1_r1 );
		_mm_store_si128((__m128i*)res_P1_Q1,res);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < 0 )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_45[i] )
					P1_H(stride) = res_P1_Q1[i];
				if( test_cond_45[i+4] )
					Q1_H(stride) = res_P1_Q1[i+4];
			}
			P++;
			Q++;
		}
	}
}

void FilterHorBS_123_intr4( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	__declspec(align(16)) __m128i P0Q0_store_cond;
	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_srai_epi16( delta_p0q0, 15 );
		adelta_p1p0 = _mm_srai_epi16( delta_p1p0, 15 );
		adelta_q1q0 = _mm_srai_epi16( delta_q1q0, 15 );
		delta_p0q0 = _mm_xor_si128( adelta_p0q0, delta_p0q0 );
		delta_p1p0 = _mm_xor_si128( adelta_p1p0, delta_p1p0 );
		delta_q1q0 = _mm_xor_si128( adelta_q1q0, delta_q1q0 );
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, adelta_p0q0 );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, adelta_p1p0 );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, adelta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		P0Q0_store_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		P0Q0_store_cond = _mm_and_si128( P0Q0_store_cond, delta_q1q0 );
		P0Q0_store_cond = _mm_slli_si128( P0Q0_store_cond, 8 );
		P0Q0_store_cond = _mm_srli_si128( P0Q0_store_cond, 8 );
		P0Q0_store_cond = _mm_packs_epi16(P0Q0_store_cond, zero);
	}/**/
	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );

		__declspec(align(16)) __m128i P1_store_cond;
		P1_store_cond = _mm_packs_epi16(p2q2p0q0beta, zero);
		P1_store_cond = _mm_and_si128( P0Q0_store_cond, P1_store_cond );
		__declspec(align(16)) __m128i Q1_store_cond;
		Q1_store_cond = _mm_srli_si128( p2q2p0q0beta, 8 );
		Q1_store_cond = _mm_packs_epi16(Q1_store_cond, zero);
		Q1_store_cond = _mm_and_si128( P0Q0_store_cond, Q1_store_cond );

		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		res_p0 = _mm_packus_epi16(res_p0, res_p0);
		res_q0 = _mm_packus_epi16(res_q0, res_q0);

		_mm_maskmoveu_si128( res_p0, P0Q0_store_cond, (char*)&P0_H(stride));
		_mm_maskmoveu_si128( res_q0, P0Q0_store_cond, (char*)&Q0_H(stride));

		__declspec(align(16)) __m128i one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep0q0one,p2_r1q2_r2,p1_r2q1_r1,p1sq1s,res;
		p1_r2q1_r1 = _mm_unpacklo_epi64(p1_r2, q1_r1);
		p0q0onep0q0one = _mm_unpacklo_epi64(p0q0one, p0q0one);
		p2_r1q2_r2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p1sq1s = _mm_slli_epi16(p1_r2q1_r1, 1);

		res = _mm_adds_epi16( p0q0onep0q0one,p2_r1q2_r2 );
		res = _mm_sub_epi16( res,p1sq1s );
		res = _mm_srai_epi16( res, 1);
		res = _mm_min_epi16( res , high_limit );
		res = _mm_max_epi16( res , low_limit );
		res = _mm_adds_epi16( res, p1_r2q1_r1 );

		res = _mm_packus_epi16(res, res);
		_mm_maskmoveu_si128( res, P1_store_cond, (char*)&P1_H(stride));
		__declspec(align(16)) __m128i res_q1;
		res_q1 = _mm_srli_si128( p2q2p0q0beta, 8 );
		_mm_maskmoveu_si128( res_q1, P0Q0_store_cond, (char*)&Q1_H(stride));

	} 
}

void FilterHorBS_123_intr5( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8],test_cond_1[16],test_cond_2[16],test_cond_3[16];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];
	__declspec(align(16)) short res_P1_Q1[16];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	{
/*
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		*/
		__declspec(align(16)) __m128i p0q0,q0p0,mpq;
		mpq = _mm_cmpgt_epi8( p0_r3, q0_r0 );
		p0q0 = _mm_sub_epi8( p0_r3, q0_r0 );
		q0p0 = _mm_sub_epi8( q0_r0, p0_r3 );
		_mm_store_si128((__m128i*)test_cond_1,p0q0);
		_mm_maskmoveu_si128( q0p0, mpq, test_cond_1);

		__declspec(align(16)) __m128i p0p1,p1p0,mpp;
		mpp = _mm_cmpgt_epi8( p0_r3, p1_r2 );
		p0p1 = _mm_sub_epi8( p0_r3, p1_r2 );
		p1p0 = _mm_sub_epi8( p1_r2, p0_r3 );
		_mm_store_si128((__m128i*)test_cond_1,p0p1);
		_mm_maskmoveu_si128( p1p0, mpp, test_cond_2);

		__declspec(align(16)) __m128i q0q1,q1q0,mqq;
		mqq = _mm_cmpgt_epi8( q0_r0, q1_r1 );
		p0q0 = _mm_sub_epi8( q0_r0, q1_r1 );
		q0p0 = _mm_sub_epi8( q1_r1, q0_r0 );
		_mm_store_si128((__m128i*)test_cond_1,p0q0);
		_mm_maskmoveu_si128( q0p0, mqq, test_cond_3);
	}

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < m_alpha && test_cond_2[i] < m_beta && test_cond_3[i] < m_beta )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_45[i] )
					P1_H(stride) = res_P1_Q1[i];
				if( test_cond_45[i+4] )
					Q1_H(stride) = res_P1_Q1[i+4];
			}
			P++;
			Q++;
		}
	}
}


void FilterHorBS_123_intr6( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) char test_cond_1[16];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];
	__declspec(align(16)) short res_P_0[8],res_Q_0[8];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	{
		__declspec(align(16)) __m128i p0q0p0;
		__declspec(align(16)) __m128i p1q1q0;
		p0q0p0 = _mm_unpacklo_epi32( p0_r3, q0_r0 );
		p1q1q0 = _mm_unpacklo_epi32( p1_r2, q1_r1 );
		p0q0p0 = _mm_unpacklo_epi64( p0q0p0, p0_r3 );
		p1q1q0 = _mm_unpacklo_epi64( p1q1q0, q0_r0 );

		__declspec(align(16)) __m128i amb,bma,mab;
		mab = _mm_cmpgt_epi8( p0q0p0, p1q1q0 );
		amb = _mm_sub_epi8( p0q0p0, p1q1q0 );
		bma = _mm_sub_epi8( p1q1q0, p0q0p0 );
		bma = _mm_andnot_si128(mab, bma);
		amb = _mm_and_si128(mab, amb);
		amb = _mm_or_si128(bma, amb);

		_mm_store_si128((__m128i*)test_cond_1,amb);
	}

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < m_beta && test_cond_1[i+4] < m_beta && test_cond_1[i+8] < m_alpha )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_45[i] )
					P1_H(stride) = res_P_1[i];
				if( test_cond_45[i+4] )
					Q1_H(stride) = res_Q_1[i+4];
			}
			P++;
			Q++;
		}
	}
}


void FilterHorBS_123_intr7( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) char test_cond_1[8];
	__declspec(align(16)) char test_cond_45[16];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8];
	__declspec(align(16)) char res_P_1[8];
	__declspec(align(16)) char res_Q_1[8];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_sub_epi16( zero, delta_p0q0 );
		adelta_p1p0 = _mm_sub_epi16( zero, delta_p1p0 );
		adelta_q1q0 = _mm_sub_epi16( zero, delta_q1q0 );

		delta_p0q0 = _mm_max_epi16( adelta_p0q0 , delta_p0q0 );
		delta_p1p0 = _mm_max_epi16( adelta_p1p0 , delta_p1p0 );
		delta_q1q0 = _mm_max_epi16( adelta_q1q0 , delta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		__declspec(align(16)) __m128i t_cond;
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		t_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		t_cond = _mm_and_si128( t_cond, delta_q1q0 );
		//if all are less then 0 then we can do full writes for P0 and Q0
		t_cond = _mm_srli_epi16( t_cond, 15 ); //get signs
		t_cond = _mm_packus_epi16(t_cond, t_cond);
		_mm_storel_epi64((__m128i*)test_cond_1,t_cond);
	}/**/
	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );
		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		p2q2p0q0beta = _mm_packus_epi16(p2q2p0q0beta, p2q2p0q0beta);
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_p0 = _mm_max_epi16( res_p0 , zero );
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		res_q0 = _mm_max_epi16( res_q0 , zero );
		res_p0 = _mm_packus_epi16(res_p0, res_p0);
		res_q0 = _mm_packus_epi16(res_q0, res_q0);
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		p0q0onep2 = _mm_packus_epi16(p0q0onep2, p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		p0q0oneq2 = _mm_packus_epi16(p0q0oneq2, p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		//there is an 25-50% chance this will trigger in real encoding scenario
		if( *(int*)test_cond_1 == 0x01010101 )
		{
			*(int*)&P0_H(stride) = *(int*)res_P_0;
			*(int*)&Q0_H(stride) = *(int*)res_Q_0;
			//there is a 90% chance these will trigger in real case
			if( *(int*)test_cond_45 == 0 && *(int*)&test_cond_45[4] == 0 )
			{
				return;
			}
			if( *(int*)test_cond_45 == 0x01010101 && *(int*)&test_cond_45[4] == 0x01010101 )
			{
				*(int*)&P1_H(stride) = *(int*)res_P_1;
				*(int*)&Q1_H(stride) = *(int*)res_Q_1;
			}
			//there is a 10% chance thsi will trigger
			else
			{
				for( int i = 0; i < 4; i++ )
				{
					if( test_cond_45[i] )
						P1_H(stride) = res_P_1[i];
					if( test_cond_45[i+4] )
						Q1_H(stride) = res_Q_1[i];
					P++;
					Q++;
				}
			}
		}
		//there is an 75-50% chance this will trigger in real encoding scenario
		else if( *(int*)test_cond_1 == 0 )
		{
			return;
		}
		//there is a 10% chance this will trigger
		else 
		{
			for( int i = 0; i < 4; i++ )
			{
				if( test_cond_1[i] != 0 )
				{
					P0_H(stride) = res_P_0[i];
					Q0_H(stride) = res_Q_0[i];
					if( test_cond_45[i] )
						P1_H(stride) = res_P_1[i];
					if( test_cond_45[i+4] )
						Q1_H(stride) = res_Q_1[i];
				}
				P++;
				Q++;
			}
		}
	}
}

void FilterHorBS_123_intr8( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
{
//	INT line;
//	INT delta_p0q0, C, DELTA;
	INT C0;
//	INT ap = 0, aq = 0;
//	BYTE *PixBuff;
	int stride = m_rec_luma_stride;
//	INT m_alpha = ALPHA_TABLE[idx_A];
//	INT m_beta = BETA_TABLE[idx_B];
//	INT *m_C0_TAB = &CLIP_TABLE[idx_A][0];
	INT bs = borderstrength;
	INT m_alpha = ALPHA_TABLE[T_IDX_A];
	INT m_beta = BETA_TABLE[T_IDX_B];
	INT *m_C0_TAB = &CLIP_TABLE[T_BS][0];

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_1[8],test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;
	p2_r1 = _mm_loadl_epi64((__m128i*)(&P2_H(stride)));
	p1_r2 = _mm_loadl_epi64((__m128i*)(&P1_H(stride)));
	p0_r3 = _mm_loadl_epi64((__m128i*)(&P0_H(stride)));

	q0_r0 = _mm_loadl_epi64((__m128i*)(&Q0_H(stride)));
	q1_r1 = _mm_loadl_epi64((__m128i*)(&Q1_H(stride)));
	q2_r2 = _mm_loadl_epi64((__m128i*)(&Q2_H(stride)));

	p2_r1 = _mm_unpacklo_epi8(p2_r1, zero);
	p1_r2 = _mm_unpacklo_epi8(p1_r2, zero);
	p0_r3 = _mm_unpacklo_epi8(p0_r3, zero);

	q0_r0 = _mm_unpacklo_epi8(q0_r0, zero);
	q1_r1 = _mm_unpacklo_epi8(q1_r1, zero);
	q2_r2 = _mm_unpacklo_epi8(q2_r2, zero);

	__declspec(align(16)) __m128i m_beta_vect;
	m_beta_vect = _mm_set1_epi16(m_beta);

	{
		__declspec(align(16)) __m128i delta_p0q0,delta_p1p0,delta_q1q0,adelta_p0q0,adelta_p1p0,adelta_q1q0;
		delta_p0q0 = _mm_sub_epi16( p0_r3, q0_r0 );
		delta_p1p0 = _mm_sub_epi16( p0_r3, p1_r2 );
		delta_q1q0 = _mm_sub_epi16( q0_r0, q1_r1 );
		//do abs
		adelta_p0q0 = _mm_sub_epi16( zero, delta_p0q0 );
		adelta_p1p0 = _mm_sub_epi16( zero, delta_p1p0 );
		adelta_q1q0 = _mm_sub_epi16( zero, delta_q1q0 );

		delta_p0q0 = _mm_max_epi16( adelta_p0q0 , delta_p0q0 );
		delta_p1p0 = _mm_max_epi16( adelta_p1p0 , delta_p1p0 );
		delta_q1q0 = _mm_max_epi16( adelta_q1q0 , delta_q1q0 );

		__declspec(align(16)) __m128i m_alpha_vect;
		m_alpha_vect = _mm_set1_epi16(m_alpha);

		__declspec(align(16)) __m128i t_cond;
		delta_p0q0 = _mm_sub_epi16( delta_p0q0, m_alpha_vect );
		delta_p1p0 = _mm_sub_epi16( delta_p1p0, m_beta_vect );
		delta_q1q0 = _mm_sub_epi16( delta_q1q0, m_beta_vect );
		t_cond = _mm_and_si128( delta_p0q0, delta_p1p0 );
		t_cond = _mm_and_si128( t_cond, delta_q1q0 );
		_mm_storel_epi64((__m128i*)test_cond_1,t_cond);
	}/**/
	{
		__declspec(align(16)) __m128i p2q2,p0q0,p2q2p0q0,ap2q2p0q0,p2q2p0q0beta;
		p2q2 = _mm_unpacklo_epi64(p2_r1, q2_r2);
		p0q0 = _mm_unpacklo_epi64(p0_r3, q0_r0);
		p2q2p0q0 = _mm_sub_epi16(p2q2, p0q0);
		ap2q2p0q0 = _mm_srai_epi16( p2q2p0q0, 15 );
		p2q2p0q0 = _mm_xor_si128( ap2q2p0q0, p2q2p0q0 );
		p2q2p0q0 = _mm_sub_epi16( p2q2p0q0, ap2q2p0q0 );

		p2q2p0q0beta = _mm_sub_epi16( p2q2p0q0, m_beta_vect );
		p2q2p0q0beta = _mm_srli_epi16( p2q2p0q0beta, 15 );	//if this is 1 then we will store result
		_mm_store_si128((__m128i*)test_cond_45,p2q2p0q0beta);

		__declspec(align(16)) __m128i delta_p2p0beta,delta_q2q0beta;
		delta_p2p0beta = p2q2p0q0beta;
		delta_q2q0beta = _mm_srli_si128( p2q2p0q0beta, 8 );

		__declspec(align(16)) __m128i C0_vect;
		C0_vect = _mm_set1_epi16(C0);

		__declspec(align(16)) __m128i C;
		C = _mm_adds_epu16(delta_p2p0beta,delta_q2q0beta);
		C = _mm_adds_epu16(C,C0_vect);

		__declspec(align(16)) __m128i DELTA,q0p0,p1q1,four;
		four = _mm_set1_epi16(4);
		q0p0 = _mm_sub_epi16(q0_r0,p0_r3);
		q0p0 = _mm_slli_epi16(q0p0, 2);
		p1q1 = _mm_sub_epi16(p1_r2,q1_r1);

		DELTA = _mm_adds_epi16(q0p0,p1q1);
		DELTA = _mm_adds_epi16(DELTA,four);
		DELTA = _mm_srai_epi16(DELTA, 3);	//!!!! shifting a negative number by 3 is not the same as dividing it by 8 !

		__declspec(align(16)) __m128i low_limit,high_limit;
		low_limit = _mm_sub_epi16( zero, C );
		high_limit = C;
		DELTA = _mm_min_epi16( DELTA , high_limit );
		DELTA = _mm_max_epi16( DELTA , low_limit );

		__declspec(align(16)) __m128i res_p0,res_q0;
		res_p0 = _mm_adds_epi16(p0_r3,DELTA);
		res_q0 = _mm_sub_epi16(q0_r0,DELTA);
		res_p0 = _mm_max_epi16( res_p0 , zero );		//clip1
		res_q0 = _mm_max_epi16( res_q0 , zero );		//clip1
		res_p0 = _mm_packus_epi16( res_p0, res_p0 );	//clip1
		res_q0 = _mm_packus_epi16( res_q0, res_q0 );	//clip1
		_mm_storel_epi64((__m128i*)res_P_0,res_p0);
		_mm_storel_epi64((__m128i*)res_Q_0,res_q0);

		__declspec(align(16)) __m128i res_p1,res_q1, one,p0q0one;
		low_limit = _mm_sub_epi16( zero, C0_vect );
		high_limit = C0_vect;
		one = _mm_set1_epi16(1);
		p0q0one = _mm_adds_epi16(p0_r3,q0_r0);
		p0q0one = _mm_adds_epi16(p0q0one,one);
		p0q0one = _mm_srli_epi16(p0q0one, 1);

		__declspec(align(16)) __m128i p0q0onep2,p1s;
		p0q0onep2 = _mm_adds_epi16(p0q0one,p2_r1);
		p1s = _mm_slli_epi16(p1_r2, 1);
		p0q0onep2 = _mm_sub_epi16(p0q0onep2,p1s);
		p0q0onep2 = _mm_srai_epi16(p0q0onep2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0onep2 = _mm_min_epi16( p0q0onep2 , high_limit );
		p0q0onep2 = _mm_max_epi16( p0q0onep2 , low_limit );
		p0q0onep2 = _mm_adds_epi16(p1_r2,p0q0onep2);
		_mm_storel_epi64((__m128i*)res_P_1,p0q0onep2);

		__declspec(align(16)) __m128i p0q0oneq2,q1s;
		p0q0oneq2 = _mm_adds_epi16(p0q0one,q2_r2);
		q1s = _mm_slli_epi16(q1_r1, 1);
		p0q0oneq2 = _mm_sub_epi16(p0q0oneq2,q1s);
		p0q0oneq2 = _mm_srai_epi16(p0q0oneq2, 1);	//!!!! shifting a negative number by 1 is not the same as dividing it by 2 !
		p0q0oneq2 = _mm_min_epi16( p0q0oneq2 , high_limit );
		p0q0oneq2 = _mm_max_epi16( p0q0oneq2 , low_limit );
		p0q0oneq2 = _mm_adds_epi16(q1_r1,p0q0oneq2);
		_mm_storel_epi64((__m128i*)res_Q_1,p0q0oneq2);

		for( int i = 0; i < 4; i++ )
		{
			if( test_cond_1[i] < 0 )
			{
				P0_H(stride) = res_P_0[i];
				Q0_H(stride) = res_Q_0[i];
				if( test_cond_45[i] )
					P1_H(stride) = res_P_1[i];
				if( test_cond_45[i+4] )
					Q1_H(stride) = res_Q_1[i];
			}
			P++;
			Q++;
		}
	}
}


#define BS 3	//borderstrength

void DoInterpolateTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned char* orio,unsigned char* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

/*	int nogood = 0;
	for(int a=1;a<255;a+=1)
		for(int b=1;b<255;b+=1)
			for(int c=1;c<255;c+=1)
			{
				int normal_res = (ABS(a-b))<c;
				int new_res = (a*b)<(c*c);
				if( normal_res != new_res )
				{
//					printf("nope : %d %d %d\n",a,b,c);
					nogood++;
				}
			}
	printf("non good version %d from total %d\n",nogood,255*255*255);
			/**/
	//just for debugging 
#define bugstart_y 0
#define bugstart_x 0
	address1 = address1 + REF_STRIDE * bugstart_y + bugstart_x;
	address2 = address2 + REF_STRIDE * bugstart_y + bugstart_x;
	orio = orio + REF_STRIDE * bugstart_y + bugstart_x;
	newo = newo + REF_STRIDE * bugstart_y + bugstart_x;
	/**/

	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int local_loop=0;local_loop<LOOP_TEST_COUNT_INTERP;local_loop++)
		{
			for( int mby = 1; mby < MB_Y_REF-2; mby++ )
				for( int mbx = 1; mbx < MB_X_REF-2; mbx++ )
				{
					BYTE* refa = address1 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					BYTE* cura = (BYTE*)orio + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					FilterHorBS_123( refa, cura, 0, BS, stride1 );
//					FilterHorBS_123_intr1( refa, cura, 0, BS, stride1 );
//					FilterHorBS_123_intr2( refa, cura, 0, BS, stride1 );
//					FilterHorBS_123_intr3( refa, cura, 0, BS, stride1 );
//printf("t");
				}
		}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int local_loop=0;local_loop<LOOP_TEST_COUNT_INTERP;local_loop++)
		{
			for( int mby = 1; mby < MB_Y_REF-2; mby++ )
				for( int mbx = 1; mbx < MB_X_REF-2; mbx++ )
				{
					BYTE* refa = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					BYTE* cura = (BYTE*)newo + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
//					FilterHorBS_123_asm( refa, cura, 0, BS, stride1 );
//					FilterHorBS_123_intr1( refa, cura, 0, BS, stride1 );
					FilterHorBS_123_intr2( refa, cura, 0, BS, stride1 );	//faster then 1,3,4,5,6
// 					FilterHorBS_123_intr3( refa, cura, 0, BS, stride1 );
//					FilterHorBS_123_intr4( refa, cura, 0, BS, stride1 );	//not good but not worth fixing ! 25% slower then other versions
//					FilterHorBS_123_intr5( refa, cura, 0, BS, stride1 );	//bad and insane slow
//					FilterHorBS_123_intr6( refa, cura, 0, BS, stride1 );	
//					FilterHorBS_123_intr7( refa, cura, 0, BS, stride1 );	// 22% faster then rev2, but it is based on statistilcal data. It might get 4% slower then rev2
//					FilterHorBS_123_intr8( refa, cura, 0, BS, stride1 );	
//printf("t");
				}
		}
/*
{
BYTE* ta = (BYTE*)orio + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
for(int ty=0;ty<MB_SIZE;ty++)
	for(int tx=0;tx<MB_SIZE;tx++)
		if( cura[ ty * REF_STRIDE + tx ] != ta [ty * REF_STRIDE + tx] )
			printf("error detected here");
}
*/
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("Old algorithm will perform statistically 25% better then here\n");
	printf("Interpolate NxN: MS for old algorithm : %u\n",diffo);
	printf("Interpolate NxN: MS for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("Interpolate: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) ) == 0 ? "true":"false");
	printf("Interpolate: Results match 2 : %s \n", memcmp( address1,address2, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) ) == 0 ? "true":"false");
	//dump results in text file for investigation
	{
		FILE *f=fopen("old.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)orio)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)newo)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("in.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
				{
					unsigned char *base = &address1[y*REF_STRIDE+x];
					fprintf(f,"%03d ",*base);
				}
				fprintf(f,"\n");
			}
			fclose(f);
		}
	}
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE* address1 = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* address2 = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* orio = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* newo = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memset( orio, 0, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memset( newo, 0, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	for(int y=0;y<2*MB_Y_REF * MB_SIZE;y++)
		for(int x=0;x<MB_X_REF * MB_SIZE;x++)
		{
			int i = y * REF_STRIDE + x;
//			address1[i] = x+y;
//			orio[i] = x+y+5;
		}
address1[(MB_SIZE +0) * REF_STRIDE + MB_SIZE + 0 ] = 251;
orio[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 0 ] = 255;
address1[(MB_SIZE +0) * REF_STRIDE + MB_SIZE + 1 ] = 252;
orio[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 1 ] = 255;
address1[(MB_SIZE +0) * REF_STRIDE + MB_SIZE + 2 ] = 255;
orio[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 2 ] = 255;
address1[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 0 ] = 249;
orio[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 0 ] = 255;
address1[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 1 ] = 249;
orio[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 1 ] = 255;
address1[(MB_SIZE +1) * REF_STRIDE + MB_SIZE + 2 ] = 249;
orio[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 2 ] = 255;
address1[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 0 ] = 174;
orio[(MB_SIZE +3) * REF_STRIDE + MB_SIZE + 0 ] = 255;
address1[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 1 ] = 174;
orio[(MB_SIZE +3) * REF_STRIDE + MB_SIZE + 1 ] = 255;
address1[(MB_SIZE +2) * REF_STRIDE + MB_SIZE + 2 ] = 174;
orio[(MB_SIZE +3) * REF_STRIDE + MB_SIZE + 2 ] = 255;

	memcpy( address2, address1, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memcpy( newo, orio, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );

	DoInterpolateTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( address1 );
	free( address2 );
	free( orio );
	free( newo );
	return 0;

}