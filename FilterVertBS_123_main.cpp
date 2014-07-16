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

#define p0	PixBuff[3]
#define p1	PixBuff[2]
#define p2	PixBuff[1]
#define p3	PixBuff[0]
#define q0	PixBuff[4]
#define q1	PixBuff[5]
#define q2	PixBuff[6]
#define q3	PixBuff[7]

#define p0s(stride)	PixBuff[3+stride]
#define p1s(stride)	PixBuff[2+stride]
#define p2s(stride)	PixBuff[1+stride]
#define p3s(stride)	PixBuff[0+stride]
#define q0s(stride)	PixBuff[4+stride]
#define q1s(stride)	PixBuff[5+stride]
#define q2s(stride)	PixBuff[6+stride]
#define q3s(stride)	PixBuff[7+stride]

#define P0_V	P[3]
#define P1_V	P[2]
#define P2_V	P[1]
#define P3_V	P[0]
#define Q0_V	Q[0]
#define Q1_V	Q[1]
#define Q2_V	Q[2]
#define Q3_V	Q[3]

#define P0_Vs(stride)	P[3+stride]
#define P1_Vs(stride)	P[2+stride]
#define P2_Vs(stride)	P[1+stride]
#define P3_Vs(stride)	P[0+stride]
#define Q0_Vs(stride)	Q[0+stride]
#define Q1_Vs(stride)	Q[1+stride]
#define Q2_Vs(stride)	Q[2+stride]
#define Q3_Vs(stride)	Q[3+stride]

void FilterVertBS_123( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
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
	BYTE *PixBuff;

	bs--;				// table is from 0..2

	C0 = m_C0_TAB[bs];
	for( line = 0; line < 4; line++ )
	{
		PixBuff = P;
		// current line
		delta_p0q0 = ABS( p0 - q0 );
		int delta_p1q0 = ABS(p1 - p0);
		int delta_q1q0 = ABS(q1 - q0);

		if( (delta_p0q0 < m_alpha) &&
			(delta_p1q0 < m_beta) &&
			(delta_q1q0 < m_beta) )
		{
			const INT pp0 = p0;
			const INT qq0 = q0;

			ap = (ABS( p2 - p0 ) - m_beta) < 0;
			aq = (ABS( q2 - q0 ) - m_beta) < 0;

			C = C0 + ap + aq;

			DELTA = CLIP3( -C, C, ( (((q0 - p0)<<2) + (p1 - q1) + 4) >> 3) );

			P0_V = CLIP1( (p0 + DELTA) );
			Q0_V = CLIP1( (q0 - DELTA) );

			if( ap )
				P1_V = p1 + CLIP3( -C0, C0, ( (p2 + ((pp0+qq0+1)>>1) - (p1<<1)) >> 1 ) );

			if( aq )
				Q1_V = q1 + CLIP3( -C0, C0, ( (q2 + ((pp0+qq0+1)>>1) - (q1<<1)) >> 1 ) );
		}

		P += m_rec_luma_stride;
		Q += m_rec_luma_stride;
	}
}

void FilterVertBS_123_intr1( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
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

	{
		__declspec(align(16)) short trasposed_input_P[3][4];
		trasposed_input_P[0][0] = P2_Vs( 0 );
		trasposed_input_P[0][1] = P2_Vs( stride );
		trasposed_input_P[0][2] = P2_Vs( 2*stride );
		trasposed_input_P[0][3] = P2_Vs( 3*stride );
		trasposed_input_P[1][0] = P1_Vs( 0 );
		trasposed_input_P[1][1] = P1_Vs( stride );
		trasposed_input_P[1][2] = P1_Vs( 2*stride );
		trasposed_input_P[1][3] = P1_Vs( 3*stride );
		trasposed_input_P[2][0] = P0_Vs( 0 );
		trasposed_input_P[2][1] = P0_Vs( stride );
		trasposed_input_P[2][2] = P0_Vs( 2*stride );
		trasposed_input_P[2][3] = P0_Vs( 3*stride );

		__declspec(align(16)) short trasposed_input_Q[3][4];
		trasposed_input_Q[0][0] = Q2_Vs( 0 );
		trasposed_input_Q[0][1] = Q2_Vs( stride );
		trasposed_input_Q[0][2] = Q2_Vs( 2*stride );
		trasposed_input_Q[0][3] = Q2_Vs( 3*stride );
		trasposed_input_Q[1][0] = Q1_Vs( 0 );
		trasposed_input_Q[1][1] = Q1_Vs( stride );
		trasposed_input_Q[1][2] = Q1_Vs( 2*stride );
		trasposed_input_Q[1][3] = Q1_Vs( 3*stride );
		trasposed_input_Q[2][0] = Q0_Vs( 0 );
		trasposed_input_Q[2][1] = Q0_Vs( stride );
		trasposed_input_Q[2][2] = Q0_Vs( 2*stride );
		trasposed_input_Q[2][3] = Q0_Vs( 3*stride );

		p2_r1 = _mm_loadl_epi64((__m128i*)(&trasposed_input_P[0]));
		p1_r2 = _mm_loadl_epi64((__m128i*)(&trasposed_input_P[1]));
		p0_r3 = _mm_loadl_epi64((__m128i*)(&trasposed_input_P[2]));

		q0_r0 = _mm_loadl_epi64((__m128i*)(&trasposed_input_Q[0]));
		q1_r1 = _mm_loadl_epi64((__m128i*)(&trasposed_input_Q[1]));
		q2_r2 = _mm_loadl_epi64((__m128i*)(&trasposed_input_Q[2]));
	}

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



void FilterVertBS_123_intr2( BYTE *Q, BYTE *P, INT blk, INT borderstrength, int m_rec_luma_stride )
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
	BYTE *PixBuff = P;

	C0 = m_C0_TAB[bs-1];
	
	__declspec(align(16)) short test_cond_1[8],test_cond_4[8],test_cond_5[8];
	__declspec(align(16)) short test_cond_45[16];
	__declspec(align(16)) char res_P_0[8],res_Q_0[8];
	__declspec(align(16)) short res_P_1[8];
	__declspec(align(16)) short res_Q_1[8];

	PixBuff = P;
	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	__declspec(align(16)) __m128i p0_r3,p1_r2,p2_r1,q2_r2,q1_r1,q0_r0,p0p0q0,q0p1q1;

	{
		__declspec(align(16)) __m128i r0,r1,r2,r3,r4,r5,r6,r7;
//		r0 = _mm_loadl_epi64((__m128i*)(&P2_Vs( 0 )));			// s03 s02 s01 s00 
//		r1 = _mm_loadl_epi64((__m128i*)(&P2_Vs( stride )));		// s13 s12 s11 s10 
//		r2 = _mm_loadl_epi64((__m128i*)(&P2_Vs( 2*stride )));	// s23 s22 s21 s20 
//		r3 = _mm_loadl_epi64((__m128i*)(&P2_Vs( 3*stride )));	// s33 s32 s31 s30 
		r0 = _mm_loadl_epi64((__m128i*)( &p2s(0) ));			// s03 s02 s01 s00 
		r1 = _mm_loadl_epi64((__m128i*)( &p2s(stride) ));		// s13 s12 s11 s10 
		r2 = _mm_loadl_epi64((__m128i*)( &p2s(stride*2) ));	// s23 s22 s21 s20 
		r3 = _mm_loadl_epi64((__m128i*)( &p2s(stride*3) ));	// s33 s32 s31 s30 
		r0 = _mm_unpacklo_epi8(r0, zero);
		r1 = _mm_unpacklo_epi8(r1, zero);
		r2 = _mm_unpacklo_epi8(r2, zero);
		r3 = _mm_unpacklo_epi8(r3, zero);

		r4 = r0;
		r0 = _mm_unpacklo_epi16(r0, r1);				// s13 s03 s12 s02 
		r4 = _mm_srli_si128(r0, 8);
		//r4 = _mm_unpackhi_epi16(r4, r1);				// s11 s01 s10 s00 
		r5 = r2;
		r5 = _mm_unpacklo_epi16(r5, r3);				// s31 s21 s30 s20 
		r2 = _mm_srli_si128(r5, 8);
		//r2 = _mm_unpackhi_epi16(r2, r3);				// s33 s23 s32 s22 
		r1 = r0;
		r0 = _mm_unpacklo_epi32(r0, r5);				// s31 s21 s11 s01 
		r1 = _mm_srli_si128(r0, 8);
		//r1 = _mm_unpackhi_epi32(r1, r5);				// s30 s20 s10 s00 
//		r3 = r4;
		r4 = _mm_unpacklo_epi32(r4, r2);				// s33 s23 s12 s03 
//		r3 = _mm_srli_si128(r4, 8);

		p2_r1 = r0;
		p1_r2 = r1;
		p0_r3 = r4;

//		r0 = _mm_loadl_epi64((__m128i*)(&Q0_Vs( 0 )));			// s03 s02 s01 s00 
//		r1 = _mm_loadl_epi64((__m128i*)(&Q0_Vs( stride )));		// s13 s12 s11 s10 
//		r2 = _mm_loadl_epi64((__m128i*)(&Q0_Vs( 2*stride )));	// s23 s22 s21 s20 
//		r3 = _mm_loadl_epi64((__m128i*)(&Q0_Vs( 3*stride )));	// s33 s32 s31 s30 
		r0 = _mm_loadl_epi64((__m128i*)( &q0s(0) ));				// s03 s02 s01 s00 
		r1 = _mm_loadl_epi64((__m128i*)( &q0s(stride) ));		// s13 s12 s11 s10 
		r2 = _mm_loadl_epi64((__m128i*)( &q0s(stride*2) ));		// s23 s22 s21 s20 
		r3 = _mm_loadl_epi64((__m128i*)( &q0s(stride*3) ));		// s33 s32 s31 s30 
		r0 = _mm_unpacklo_epi8(r0, zero);
		r1 = _mm_unpacklo_epi8(r1, zero);
		r2 = _mm_unpacklo_epi8(r2, zero);
		r3 = _mm_unpacklo_epi8(r3, zero);

		r4 = r0;
		r0 = _mm_unpacklo_epi16(r0, r1);				// s13 s03 s12 s02 
		r4 = _mm_srli_si128(r0, 8);
		//p4 = _mm_unpackhi_epi16(p4, p1);				// s11 s01 s10 s00 
		r5 = r2;
		r5 = _mm_unpacklo_epi16(r5, r3);				// s31 s21 s30 s20 
		r2 = _mm_srli_si128(r5, 8);
		//p2 = _mm_unpackhi_epi16(p2, p3);				// s33 s23 s32 s22 
		r1 = r0;
		r0 = _mm_unpacklo_epi32(r0, r5);				// s31 s21 s11 s01 
		r1 = _mm_srli_si128(r0, 8);
		//p1 = _mm_unpackhi_epi32(p1, p5);				// s30 s20 s10 s00 
//		r3 = r4;
		r4 = _mm_unpacklo_epi32(r4, r2);				// s33 s23 s12 s03 
//		r3 = _mm_srli_si128(r4, 8);

		q0_r0 = r0;
		q1_r1 = r1;
		q2_r2 = r4;
	}

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
				P0_V = res_P_0[i];
				Q0_V = res_Q_0[i];
				if( test_cond_45[i] )
					P1_V = res_P_1[i];
				if( test_cond_45[i+4] )
					Q1_V = res_Q_1[i];
			}
			P+=stride;
			Q+=stride;
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
					FilterVertBS_123( refa, cura, 0, BS, stride1 );
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
					FilterVertBS_123_intr2( refa, cura, 0, BS, stride1 );
				}
		}
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
		FILE *f=fopen("old_p.txt","wt");
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
		f=fopen("new_p.txt","wt");
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
		f=fopen("old_q.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)address1)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new_q.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)address2)[y*REF_STRIDE+x]);
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
	{
		for(int x=0;x<MB_X_REF * MB_SIZE;x++)
		{
			int i = y * REF_STRIDE + x;
			address1[i] = x+y*2;
			orio[i] = x+y*2+5;
		}
	}

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