#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>
//#include <tmmintrin.h>

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT			1	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT			50	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	500	//to have a larger runtime
#endif

#define BLOCK_SIZE	 4

void Block_4x4_DCT_c1( short *input, int TEXTURE_BUFFER_STRIDE )
{
	INT i;
	INT z0, z1, z2, z3;
	short *x0, *x1, *x2, *x3;
	short *s = input;

	// horizontal
	for( i = 0; i < BLOCK_SIZE; i++)
	{
		z0 = s[0] + s[3];
		z1 = s[1] + s[2];
		z2 = s[1] - s[2];
		z3 = s[0] - s[3];

		s[0] = z0 + z1;
		s[1] = (z3 << 1) + z2;
		s[2] = z0 - z1;
		s[3] = z3 - (z2 << 1);

		s += TEXTURE_BUFFER_STRIDE;
	}

	// vertical
	for( i = 0; i < BLOCK_SIZE; i++ )
	{
		x0 = input + 0*TEXTURE_BUFFER_STRIDE;
		x1 = input + 1*TEXTURE_BUFFER_STRIDE;
		x2 = input + 2*TEXTURE_BUFFER_STRIDE;
		x3 = input + 3*TEXTURE_BUFFER_STRIDE;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = (z3 << 1) + z2;
		*x2 = z0 - z1;
		*x3 = z3 - (z2 << 1);

		input++;
	}
}

void Block_4x4_DCT_c2( short *input, int TEXTURE_BUFFER_STRIDE )
{
	INT i;
	INT z0, z1, z2, z3;
	short *x0, *x1, *x2, *x3;
	short *s = input;

	// horizontal
	for( i = 0; i < BLOCK_SIZE; i++)
	{
		z0 = s[0] + s[3];
		z1 = s[1] + s[2];
		z2 = s[1] - s[2];
		z3 = s[0] - s[3];

		s[0] = z0 + z1;
		s[1] = z3 + z2;
		s[2] = z0 - z1;
		s[3] = z3 - z2;

		s += TEXTURE_BUFFER_STRIDE;
	}

	// vertical
	for( i = 0; i < BLOCK_SIZE; i++ )
	{
		x0 = input + 0*TEXTURE_BUFFER_STRIDE;
		x1 = input + 1*TEXTURE_BUFFER_STRIDE;
		x2 = input + 2*TEXTURE_BUFFER_STRIDE;
		x3 = input + 3*TEXTURE_BUFFER_STRIDE;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = z3 + z2;
		*x2 = z0 - z1;
		*x3 = z3 - z2;

		input++;
	}
}

void Block_4x4_DCT_c3( short *input, int TEXTURE_BUFFER_STRIDE )
{
	INT i;
	INT z0, z1, z2, z3;
	short *x0, *x1, *x2, *x3;
	short *s = input;

	// vertical
	for( i = 0; i < BLOCK_SIZE; i++ )
	{
		x0 = input + 0*TEXTURE_BUFFER_STRIDE;
		x1 = input + 1*TEXTURE_BUFFER_STRIDE;
		x2 = input + 2*TEXTURE_BUFFER_STRIDE;
		x3 = input + 3*TEXTURE_BUFFER_STRIDE;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = (z3 << 1) + z2;
		*x2 = z0 - z1;
		*x3 = z3 - (z2 << 1);

		input++;
	}

	input = s;
	// horizontal
	for( i = 0; i < BLOCK_SIZE; i++)
	{
		x0 = input + 0;
		x1 = input + 1;
		x2 = input + 2;
		x3 = input + 3;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = (z3 << 1) + z2;
		*x2 = z0 - z1;
		*x3 = z3 - (z2 << 1);

		input += TEXTURE_BUFFER_STRIDE;
	}
}

void Block_4x4_DCT_c4( short *input, int TEXTURE_BUFFER_STRIDE )
{
	INT i;
	INT z0, z1, z2, z3;
	short *x0, *x1, *x2, *x3;
	short *s = input;

	// vertical
	for( i = 0; i < BLOCK_SIZE; i++ )
	{
		x0 = input + 0*TEXTURE_BUFFER_STRIDE;
		x1 = input + 1*TEXTURE_BUFFER_STRIDE;
		x2 = input + 2*TEXTURE_BUFFER_STRIDE;
		x3 = input + 3*TEXTURE_BUFFER_STRIDE;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = z3 + z2;
		*x2 = z0 - z1;
		*x3 = z3 - z2;
		input++;
	}

	input = s;
	// horizontal
	for( i = 0; i < BLOCK_SIZE; i++)
	{
		x0 = input + 0;
		x1 = input + 1;
		x2 = input + 2;
		x3 = input + 3;

		z0 = *x0 + *x3;
		z1 = *x1 + *x2;
		z2 = *x1 - *x2;
		z3 = *x0 - *x3;

		*x0 = z0 + z1;
		*x1 = z3 + z2;
		*x2 = z0 - z1;
		*x3 = z3 - z2;

		input += TEXTURE_BUFFER_STRIDE;
	}
}

void Block_4x4_DCT_intr1( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3, p4, p5, p6, p7;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p2 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p1 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));
	p0 = _mm_unpacklo_epi64(p0, p2);
	p1 = _mm_unpacklo_epi64(p1, p3);

    p1 = _mm_shuffle_epi32( p1, 0x4e );
    p2 = p0;
    p0 = _mm_adds_epi16( p0, p1 );
    p2 = _mm_subs_epi16( p2, p1 );

    p1 = _mm_shuffle_epi32( p0, 0x4e );
    p3 = _mm_shuffle_epi32( p2, 0x4e );
    p4 = _mm_adds_epi16( p0, p1 );
    p5 = _mm_adds_epi16( p3, p2 );
    p6 = _mm_subs_epi16( p0, p1 );
    p7 = _mm_subs_epi16( p2, p3 );
    p5 = _mm_adds_epi16( p5, p2 );
    p7 = _mm_subs_epi16( p7, p3 );

//Transpose matrix
    p4 = _mm_unpacklo_epi16( p4, p5 );
    p6 = _mm_unpacklo_epi16( p6, p7 );
    p0 = p4;
    p4 = _mm_unpacklo_epi32( p4, p6 );
    p0 = _mm_unpackhi_epi32( p0, p6 );
//Matrix post mult
    p0 = _mm_shuffle_epi32( p0, 0x4e );
    p2 = p4;
    p4 = _mm_adds_epi16( p4, p0 );
    p2 = _mm_subs_epi16( p2, p0 );
    p0 = _mm_shuffle_epi32( p4, 0x4e );
    p3 = _mm_shuffle_epi32( p2, 0x4e );

    p6 = _mm_subs_epi16( p4, p0 );
    p5 = _mm_adds_epi16( p3, p2 );
    p4 = _mm_adds_epi16( p4, p0 );
    p7 = _mm_subs_epi16( p2, p3 );
    p5 = _mm_adds_epi16( p5, p2 );
    p7 = _mm_subs_epi16( p7, p3 );
//Transpose matrix
    p4 = _mm_unpacklo_epi16( p4, p5 );
    p6 = _mm_unpacklo_epi16( p6, p7 );
    p0 = p4;
    p4 = _mm_unpacklo_epi32( p4, p6 );
    p0 = _mm_unpackhi_epi32( p0, p6 );

	_mm_storel_epi64((__m128i*)(input), p4);
	p4 = _mm_srli_si128(p4, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p4);

	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p0);
	p0 = _mm_srli_si128(p0, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p0);
}

void Block_4x4_DCT_ASM( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) char hmul_4p[16]={1, 1, 1, 1, 1, -1, 1, -1,1, 1, 1, 1, 1, -1, 1, -1};
	__declspec(align(16)) int mask_10[8]={0, -1, 0, -1,0, -1,0, -1};
	__declspec(align(16)) int _x264_pw_1[8]={1,1,1,1,1,1,1,1};
	__asm {
	mov eax, input
	mov ecx, TEXTURE_BUFFER_STRIDE

	lea edi, [2*ecx]
	add edi, ecx

	movaps xmm4, [hmul_4p]
	movd xmm2, [eax]
	movd xmm5, [eax+ecx]
	movlhps xmm2, xmm5
	movsldup xmm2, xmm2
	movd xmm3, [eax+2*ecx]
	movd xmm5, [eax+edi]
	movlhps xmm3, xmm5
	movsldup xmm3, xmm3

	;movd xmm0, [eax]
	;movd xmm5, [eax+ecx]
	;movlhps xmm0, xmm5
	;movsldup xmm0, xmm0
	;movd xmm1, [eax+2*ecx]
	;movd xmm5, [eax+esi]
	;movlhps xmm1, xmm5
	;movsldup xmm1, xmm1

	pxor xmm0, xmm0
	pxor xmm1, xmm1
	pxor xmm5, xmm5

	pmaddubsw xmm2, xmm4
	pmaddubsw xmm0, xmm4
	pmaddubsw xmm3, xmm4
	pmaddubsw xmm1, xmm4
	psubw xmm0, xmm2
	psubw xmm1, xmm3
	movaps xmm2, xmm0
	paddw xmm0, xmm1
	psubw xmm1, xmm2
	movaps xmm2, xmm0
	movlhps xmm0, xmm1
	punpckhqdq xmm2, xmm1
	movaps xmm1, xmm0
	paddw xmm0, xmm2
	psubw xmm2, xmm1
	movaps xmm1, [mask_10]
	movaps xmm3, xmm2
	pslld xmm2, 16
	pand xmm3, xmm1
	pandn xmm1, xmm0
	psrld xmm0, 16
	por xmm2, xmm1
	por xmm0, xmm3
	pabsw xmm0, xmm0
	pabsw xmm2, xmm2
	pmaxsw xmm0, xmm2
	pmaddwd xmm0, [_x264_pw_1]
	movhlps xmm2, xmm0
	paddd xmm0, xmm2
	pshuflw xmm2, xmm0, 0xE
	paddd xmm0, xmm2
	movd eax, xmm0
	}
}
/*
void Block_4x4_DCT_intr2( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	__declspec(align(16)) __m128i p01,p32;
	p01 = _mm_unpacklo_epi64(p0, p1);
	p32 = _mm_unpacklo_epi64(p3, p2);

	__declspec(align(16)) __m128i p01_2,p32_2;
	// step 1
	// p0 = p0 + p3
	// p1 = p1 + p2
	// p2 = p1 - p2
	// p3 = p0 - p3
    p01_2 = _mm_adds_epi16( p01, p32 );
    p32_2 = _mm_subs_epi16( p01, p32 );

	// step 2 
	// p0 = p0 + p1
	// p1 = 2*p3 + p2
	// p2 = p0 - p1
	// p3 = p3 - 2*p2
	__declspec(align(16)) __m128i p23_2;
	__declspec(align(16)) __m128i p03,p12;
	p03 = _mm_unpacklo_epi64(p01_2, p32_2);
	p12 = _mm_unpackhi_epi64(p01_2, p32_2);

    p01_2 = _mm_adds_epi16( p03, p12 );
 //p01_2 = _mm_adds_epi16( p01_2, p3 );
	p23_2 = _mm_subs_epi16( p03, p12 );

	//transpose values from row to column or try to use SSSE intrinsics
	//
	// step 3
	// p0 = p0 + p3
	// p1 = p1 + p2
	// p2 = p1 - p2
	// p3 = p0 - p3
#define Local__MM_SHUFFLE(a,b,c,d) ((a<<6)|(b<<4)|(c<<2)|(d))
	__declspec(align(16)) __m128i c0,c1,c2,c3,c01,c23;
	c1 = _mm_shufflehi_epi16( p01_2, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c01 = _mm_shufflelo_epi16( c1, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c3 = _mm_shufflehi_epi16( p23_2, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c23 = _mm_shufflelo_epi16( c3, Local__MM_SHUFFLE( 2,1,3,0 ) );

	__declspec(align(16)) __m128i i_0123_01010101,i_0123_32323232;
	//row 0,1 and col 0,1     row 2,3 and col 0,1
	i_0123_01010101 = _mm_hadd_epi16( c01, c23 );	
	//row 0,1 and col 3,2     row 2,3 and col 3,2
	i_0123_32323232 = _mm_hsub_epi16( c01, c23 );	
	//results
	__declspec(align(16)) __m128i c00001111,c22223333;
	c00001111 = _mm_hadd_epi16( i_0123_01010101, i_0123_32323232 );
	c22223333 = _mm_hsub_epi16( i_0123_01010101, i_0123_32323232 );

	//transpose result
	// 00 10 20 30 01 11 21 31
	// 02 12 22 32 03 13 23 33
	c0 = c00001111;
	c1 = _mm_srli_si128(c00001111, 8);
	c2 = c22223333;
	c3 = _mm_srli_si128(c22223333, 8);

	__declspec(align(16)) __m128i p23;
	p01 = _mm_unpacklo_epi16(c0, c1);
	p23 = _mm_unpacklo_epi16(c2, c3);

	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	//store result
	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2 );
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2 );
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2 );
}*/

void Block_4x4_DCT_intr3( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3, p4, p5, p6, p7;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	__declspec(align(16)) __m128i p01,p32;
	p01 = _mm_unpacklo_epi64(p0, p1);
	p32 = _mm_unpacklo_epi64(p3, p2);

	__declspec(align(16)) __m128i p01_2,p32_2;
	// step 1
    p01_2 = _mm_adds_epi16( p01, p32 );
    p32_2 = _mm_subs_epi16( p01, p32 );

	// step 2 
	__declspec(align(16)) __m128i p23_2;
	__declspec(align(16)) __m128i p03,p12;
	p03 = _mm_unpacklo_epi64(p01_2, p32_2);
	p12 = _mm_unpackhi_epi64(p01_2, p32_2);

    p01_2 = _mm_adds_epi16( p03, p12 );
	p23_2 = _mm_subs_epi16( p03, p12 );

	//transpose
	p0 = p01_2;
	p1 = _mm_srli_si128(p01_2, 8);
	p2 = p23_2;
	p3 = _mm_srli_si128(p23_2, 8);

	__declspec(align(16)) __m128i p23;
	p01 = _mm_unpacklo_epi16(p0, p1);
	p32 = _mm_unpacklo_epi16(p3, p2);

	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p32_2 = _mm_unpackhi_epi32(p32, p01);

	// step 3
    p01 = _mm_adds_epi16( p01_2, p32_2 );
    p32 = _mm_subs_epi16( p01_2, p32_2 );
	// step 4
	p03 = _mm_unpacklo_epi64(p01, p32);
	p12 = _mm_unpackhi_epi64(p01, p32);

    p01_2 = _mm_adds_epi16( p03, p12 );
	p23_2 = _mm_subs_epi16( p03, p12 );

	//transpose result
	// 00 10 20 30 01 11 21 31
	// 02 12 22 32 03 13 23 33
	__declspec(align(16)) __m128i c0,c1,c2,c3,c01,c23;
	c0 = p01_2;
	c1 = _mm_srli_si128(p01_2, 8);
	c2 = p23_2;
	c3 = _mm_srli_si128(p23_2, 8);

	p01 = _mm_unpacklo_epi16(c0, c1);
	p23 = _mm_unpacklo_epi16(c2, c3);

	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2 );
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2 );
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2 );
}
/*
void Block_4x4_DCT_intr4( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;
	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	__declspec(align(16)) __m128i p01,p32;
	p01 = _mm_unpacklo_epi64(p0, p1);
	p32 = _mm_unpacklo_epi64(p3, p2);

	__declspec(align(16)) __m128i p01_2,p32_2;
	// step 1
    p01_2 = _mm_adds_epi16( p01, p32 );
    p32_2 = _mm_subs_epi16( p01, p32 );

	// step 2 
	__declspec(align(16)) __m128i p23_2;
	__declspec(align(16)) __m128i p03,p12;
	p03 = _mm_unpacklo_epi64(p01_2, p32_2);
	p12 = _mm_unpackhi_epi64(p01_2, p32_2);

	__declspec(align(16)) __m128i pz3,pz2;
    p01_2 = _mm_adds_epi16( p03, p12 );
	pz3 = _mm_unpackhi_epi64( zero, p03 );
	p01_2 = _mm_adds_epi16( p01_2, pz3 );
	p23_2 = _mm_subs_epi16( p03, p12 );
	pz2 = _mm_unpackhi_epi64( zero, p12 );
	p23_2 = _mm_subs_epi16( p23_2, pz2 );

	//transpose values from row to column or try to use SSSE intrinsics
	//
	// step 3
#define Local__MM_SHUFFLE(a,b,c,d) ((a<<6)|(b<<4)|(c<<2)|(d))
	__declspec(align(16)) __m128i c0,c1,c2,c3,c01,c23;
	c1 = _mm_shufflehi_epi16( p01_2, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c01 = _mm_shufflelo_epi16( c1, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c3 = _mm_shufflehi_epi16( p23_2, Local__MM_SHUFFLE( 2,1,3,0 ) );
	c23 = _mm_shufflelo_epi16( c3, Local__MM_SHUFFLE( 2,1,3,0 ) );

	__declspec(align(16)) __m128i i_0123_01010101,i_0123_32323232;
	//row 0,1 and col 0,1     row 2,3 and col 0,1
	i_0123_01010101 = _mm_hadd_epi16( c01, c23 );	
	//row 0,1 and col 3,2     row 2,3 and col 3,2
	i_0123_32323232 = _mm_hsub_epi16( c01, c23 );	
	//results
	__declspec(align(16)) __m128i c00001111,c22223333,i_0123_32323232_1,i_0123_32323232_2;

	pz3 =  _mm_slli_epi32( i_0123_32323232, 16 );
	pz3 =  _mm_srli_epi32( pz3, 16 );
	pz2 =  _mm_srli_epi32( i_0123_32323232, 16 );
	pz2 =  _mm_slli_epi32( pz2, 16 );

	i_0123_32323232_1 = _mm_adds_epi16( i_0123_32323232, pz3 );
	i_0123_32323232_2 = _mm_adds_epi16( i_0123_32323232, pz2 );

	c00001111 = _mm_hadd_epi16( i_0123_01010101, i_0123_32323232_1 );
	c22223333 = _mm_hsub_epi16( i_0123_01010101, i_0123_32323232_2 );

	//transpose result
	// 00 10 20 30 01 11 21 31
	// 02 12 22 32 03 13 23 33
	c0 = c00001111;
	c1 = _mm_srli_si128(c00001111, 8);
	c2 = c22223333;
	c3 = _mm_srli_si128(c22223333, 8);

	__declspec(align(16)) __m128i p23;
	p01 = _mm_unpacklo_epi16(c0, c1);
	p23 = _mm_unpacklo_epi16(c2, c3);

	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	//store result
	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2 );
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2 );
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2 );
}*/

void Block_4x4_DCT_intr5( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;
	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	__declspec(align(16)) __m128i mp1,mp2;
	mp1 = _mm_subs_epi16( zero, p1 );
	mp2 = _mm_subs_epi16( zero, p2 );

	__declspec(align(16)) __m128i p00,p33,p1m1,p2m2;
	p00 = _mm_unpacklo_epi64(p0, p0);
	p33 = _mm_unpacklo_epi64(p1, p1);
	p1m1 = _mm_unpacklo_epi64(p1, mp1);
	p2m2 = _mm_unpacklo_epi64(p2, mp2);

	__declspec(align(16)) __m128i ac,bd;
	ac = _mm_adds_epi16( p00, p1m1 );
	bd = _mm_adds_epi16( p33, p2m2 );

	__declspec(align(16)) __m128i s2_02,s2_13,t1,t2;
	s2_02 = _mm_adds_epi16( ac, bd );
	s2_13 = _mm_subs_epi16( ac, bd );

	t1 = _mm_subs_epi16( p0, p3 );
	t2 = _mm_subs_epi16( p2, p1 );
	t1 = _mm_unpacklo_epi64(t1, t2);

	s2_13 = _mm_adds_epi16( s2_13, t1 );

	//store result
	_mm_storel_epi64((__m128i*)(input), s2_02);
	s2_02 = _mm_srli_si128(s2_02, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), s2_02 );
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), s2_13 );
	s2_13 = _mm_srli_si128(s2_13, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), s2_13 );
}

//!!!temp
void Block_4x4_DCT_intr6( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3, p4, p5, p6, p7;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p2 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p1 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));
	p0 = _mm_unpacklo_epi64(p0, p2);
	p1 = _mm_unpacklo_epi64(p1, p3);

    p1 = _mm_shuffle_epi32( p1, 0x4e );
    p2 = p0;
    p0 = _mm_adds_epi16( p0, p1 );
    p2 = _mm_subs_epi16( p2, p1 );

    p1 = _mm_shuffle_epi32( p0, 0x4e );
    p3 = _mm_shuffle_epi32( p2, 0x4e );
    p4 = _mm_adds_epi16( p0, p1 );
    p5 = _mm_adds_epi16( p3, p2 );
    p6 = _mm_subs_epi16( p0, p1 );
    p7 = _mm_subs_epi16( p2, p3 );
    p5 = _mm_adds_epi16( p5, p2 );
    p7 = _mm_subs_epi16( p7, p3 );

	_mm_storel_epi64((__m128i*)(input), p4);
//	p4 = _mm_srli_si128(p4, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p5);

	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p6);
//	p0 = _mm_srli_si128(p0, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p7);
}


void Block_4x4_DCT_intr7( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	//step1
	__declspec(align(16)) __m128i p0_, p1_, p2_, p3_;
	p0_ = _mm_adds_epi16( p0, p3 );
    p1_ = _mm_adds_epi16( p1, p2 );
    p2_ = _mm_subs_epi16( p1, p2 );
    p3_ = _mm_subs_epi16( p0, p3 );
	//step2
	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_ );
    p1 = _mm_adds_epi16( p1, p3_ );
    p3 = _mm_subs_epi16( p3, p2_ );
	//transpose
	__declspec(align(16)) __m128i p01,p23,p01_2,p23_2,p32,p32_2;
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);
	//step 3
	p32_2 = _mm_shuffle_epi32( p23_2, 0x4e );

	p01 = _mm_adds_epi16(p01_2, p32_2);
	p32 = _mm_subs_epi16(p01_2, p32_2);
	//extract
//	p0_ = p01;
	p1_ = _mm_srli_si128(p01, 8);
//	p3_ = p32;
	p2_ = _mm_srli_si128(p32, 8);
	//step 4
	p0 = _mm_adds_epi16( p01, p1_ );
    p1 = _mm_adds_epi16( p32, p2_ );
    p2 = _mm_subs_epi16( p01, p1_ );
    p3 = _mm_subs_epi16( p32, p2_ );
    p1 = _mm_adds_epi16( p1, p32 );
    p3 = _mm_subs_epi16( p3, p2_ );
	//transpose
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2);
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2);
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2);
}
void Block_4x4_DCT_intr8( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	//step1
	__declspec(align(16)) __m128i p0_, p1_, p2_, p3_;
	p0_ = _mm_adds_epi16( p0, p3 );
    p1_ = _mm_adds_epi16( p1, p2 );
    p2_ = _mm_subs_epi16( p1, p2 );
    p3_ = _mm_subs_epi16( p0, p3 );
	//step2
	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_ );
    p2 = _mm_adds_epi16( p2, p3_ );
    p3 = _mm_subs_epi16( p3, p2_ );
	//transpose
	__declspec(align(16)) __m128i p01,p32,p01_2,p32_2;
	p01 = _mm_unpacklo_epi16(p0, p1);
	p32 = _mm_unpacklo_epi16(p3, p2);
	p01_2 = _mm_unpacklo_epi32(p01, p32);
	p32_2 = _mm_unpackhi_epi32(p01, p32);
	//step 3
	p01 = _mm_adds_epi16(p01_2, p32_2);
	p32 = _mm_subs_epi16(p01_2, p32_2);
	//step 4
	__declspec(align(16)) __m128i p03_2,p12_2;
	p03_2 = _mm_unpacklo_epi16(p01, p32);
	p12_2 = _mm_unpackhi_epi16(p01, p32);

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();
	p3_ = _mm_unpacklo_epi64(p01, zero);
	p2_ = _mm_unpacklo_epi64(p32, zero);
//	p3_ = _mm_srli_si128(p01, 8);
//	p2_ = _mm_srli_si128(p32, 8);
//	p3_ = _mm_slli_si128(p3_, 8);
//	p2_ = _mm_slli_si128(p2_, 8);

	p01 = _mm_adds_epi16(p03_2, p12_2);
	p32 = _mm_subs_epi16(p03_2, p12_2);

	p01 = _mm_adds_epi16(p01, p3_);
	p32 = _mm_subs_epi16(p32, p2_);

	p0_ = p01;
	p1_ = _mm_srli_si128(p01, 8);
	p2_ = _mm_srli_si128(p32, 8);
	p3_ = p32;

	//transpose
	p01 = _mm_unpacklo_epi16(p0, p1);
	p32 = _mm_unpacklo_epi16(p3, p2);
	p01_2 = _mm_unpacklo_epi32(p01, p32);
	p32_2 = _mm_unpackhi_epi32(p01, p32);

	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2);
	p2 = _mm_srli_si128(p32_2, 8);
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p2);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p32_2);
}
void Block_4x4_DCT_intr9( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	//step1
	__declspec(align(16)) __m128i p0_, p1_, p2_, p3_;
	p0_ = _mm_adds_epi16( p0, p3 );
    p1_ = _mm_adds_epi16( p1, p2 );
    p2_ = _mm_subs_epi16( p1, p2 );
    p3_ = _mm_subs_epi16( p0, p3 );
	//step2
	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_ );
    p1 = _mm_adds_epi16( p1, p3_ );
    p3 = _mm_subs_epi16( p3, p2_ );
	//transpose
	__declspec(align(16)) __m128i p01,p23,p01_2,p23_2,p32,p32_2;
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

//	p0 = p01_2;
	p1 = _mm_srli_si128(p01_2, 8);
	p3 = _mm_srli_si128(p23_2, 8);
//	p2 = p23_2;

	//step 3
	p0_ = _mm_adds_epi16( p01_2, p3 );
    p1_ = _mm_adds_epi16( p1, p23_2 );
    p2_ = _mm_subs_epi16( p1, p23_2 );
    p3_ = _mm_subs_epi16( p01_2, p3 );
	//step 4
	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_ );
    p1 = _mm_adds_epi16( p1, p3_ );
    p3 = _mm_subs_epi16( p3, p2_ );

	//transpose
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2);
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2);
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2);
}
void Block_4x4_DCT_intr10( short *input, int TEXTURE_BUFFER_STRIDE )
{
	__declspec(align(16)) __m128i p0, p1, p2, p3;

	p0 = _mm_loadl_epi64((__m128i*)(input));
	p1 = _mm_loadl_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE));
	p2 = _mm_loadl_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE));
	p3 = _mm_loadl_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE));

	//step1
	__declspec(align(16)) __m128i p0_, p1_, p2_, p3_;
	p0_ = _mm_adds_epi16( p0, p3 );
    p1_ = _mm_adds_epi16( p1, p2 );
    p2_ = _mm_subs_epi16( p1, p2 );
    p3_ = _mm_subs_epi16( p0, p3 );
	//step2
	__declspec(align(16)) __m128i p3_2,p2_2;
	p3_2 = _mm_slli_epi16(p3_, 1);
	p2_2 = _mm_slli_epi16(p2_, 1);

	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_2, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_2 );

	//transpose
	__declspec(align(16)) __m128i p01,p23,p01_2,p23_2,p32,p32_2;
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	p1 = _mm_srli_si128(p01_2, 8);
	p3 = _mm_srli_si128(p23_2, 8);

	//step 3
	p0_ = _mm_adds_epi16( p01_2, p3 );
    p1_ = _mm_adds_epi16( p1, p23_2 );
    p2_ = _mm_subs_epi16( p1, p23_2 );
    p3_ = _mm_subs_epi16( p01_2, p3 );
	//step 4
	p3_2 = _mm_slli_epi16(p3_, 1);
	p2_2 = _mm_slli_epi16(p2_, 1);

	p0 = _mm_adds_epi16( p0_, p1_ );
    p1 = _mm_adds_epi16( p3_2, p2_ );
    p2 = _mm_subs_epi16( p0_, p1_ );
    p3 = _mm_subs_epi16( p3_, p2_2 );

	//transpose
	p01 = _mm_unpacklo_epi16(p0, p1);
	p23 = _mm_unpacklo_epi16(p2, p3);
	p01_2 = _mm_unpacklo_epi32(p01, p23);
	p23_2 = _mm_unpackhi_epi32(p01, p23);

	_mm_storel_epi64((__m128i*)(input), p01_2);
	p01_2 = _mm_srli_si128(p01_2, 8);
	_mm_storel_epi64((__m128i*)(input+TEXTURE_BUFFER_STRIDE), p01_2);
	_mm_storel_epi64((__m128i*)(input+2*TEXTURE_BUFFER_STRIDE), p23_2);
	p23_2 = _mm_srli_si128(p23_2, 8);
	_mm_storel_epi64((__m128i*)(input+3*TEXTURE_BUFFER_STRIDE), p23_2);
}

#define bugaty 0
#define bugatx 0

void DoSATDTest(short* address1,int stride1, short* address2,int stride2)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	//matches 1 - 3
	//2 - 4
	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = bugaty; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				short* cura = address1 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
				for( int loop=0;loop<LOOP_TEST_COUNT_INTERP;loop++)
				{
					Block_4x4_DCT_intr1( (short*)cura, stride1 );
//					Block_4x4_DCT_intr7( (short*)cura, stride1 );
//					Block_4x4_DCT_c1( (short*)cura, stride1 );
//					Block_4x4_DCT_c3( (short*)cura, stride1 );
//					Block_4x4_DCT_intr2( (short*)cura, stride1 );
//					Block_4x4_DCT_intr9( (short*)cura, 16 );	//this version is only for our encoder specific
				}
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int mby = bugaty; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				short* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
				for( int loop=0;loop<LOOP_TEST_COUNT_INTERP;loop++)
				{
//					Block_4x4_DCT( (short*)cura, stride2 );
//					Block_4x4_DCT_intr2( (short*)cura, stride1 );
//					Block_4x4_DCT_intr3( (short*)cura, stride1 );	//not good and slower then ver 2
//					Block_4x4_DCT_intr7( (short*)cura, stride1 );
//					Block_4x4_DCT_intr8( (short*)cura, stride1 );	//slower then ver 7
//					Block_4x4_DCT_intr9( (short*)cura, stride1 );	//slightly better then rev 7
//					Block_4x4_DCT_intr9( (short*)cura, 16 );	//our encoder versions specific only !
					Block_4x4_DCT_intr10( (short*)cura, stride1 );	//slightly better then rev9, but it is sensible to input data !
//					Block_4x4_DCT_c2( (short*)cura, stride1 );
//					Block_4x4_DCT_c1( (short*)cura, stride1 );
				}
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	int run_count = MB_Y_REF * MB_X_REF * LOOP_TEST_COUNT_INTERP;
	int duration_N_runs_old = diffo / ( run_count / 1000000 );
	int duration_N_runs_new = diffn / ( run_count / 1000000 );
	printf("DCT NxN: MS for old algorithm : %u\n",diffo);
	printf("DCT NxN: MS for new algorithm : %u\n",diffn);
	printf("duration for N runs of old function %d\n",duration_N_runs_old);
	printf("duration for N runs of new function %d\n",duration_N_runs_new);
#ifdef _DEBUG
	//in release mode the values will loop and the result is not the same. This should not happen for real input where values are known to be small enough to not cause loops
	printf("DCT: Results match : %s \n", memcmp( address1,address2, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) ) == 0 ? "true":"false");
#endif

	//dump results in text file for investigation
	{
		FILE *f=fopen("old.txt","wt");
		if( f )
		{
			for(int y=bugaty*MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=0;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%06d ",address1[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new.txt","wt");
		if( f )
		{
			for(int y=bugaty*MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=0;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%06d ",address2[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
	}
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	short* address1 = (short*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( short ) );
	short* address2 = (short*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( short ) );
	memset( address1, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( short ) );
//	memset( address2, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE;i++)
		address1[i] = i % 4000;

	memcpy( address2, address1, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( short ) );

	DoSATDTest( address1, stride, address2, stride );

	getch();
	free( address1 );
	free( address2 );
	return 0;

}