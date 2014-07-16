#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#define LOOP_TEST_COUNT	30	//to have a larger runtime
#define LOOP_TEST_COUNT_INTRA	1	//to have a larger runtime
#ifdef _DEBUG
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT_INTERP	50	//to have a larger runtime
#endif

extern "C" unsigned int satd_4x4_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" unsigned int satd_4x4_sse4_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

extern "C" unsigned int satd_4x4_sse3_YASM_CACHED( unsigned char *address1, unsigned char *address2 );
extern "C" unsigned int satd_4x4_sse4_YASM_CACHED( unsigned char *address1, unsigned char *address2 );

extern "C" unsigned int sad_4x4_YASM_CACHED( unsigned char *address1, unsigned char *address2 );
extern unsigned int SATD_4x4_CACHED( unsigned char *address1, unsigned char *address2 );

extern "C" unsigned int satd_8x8_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" unsigned int satd_8x8_sse4_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

extern unsigned int SATD16x16_INTEL_8x8_C( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern unsigned int SATD_16x16_SSE2_Intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" unsigned int satd_16x16_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" unsigned int satd_16x16_sse4_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

extern unsigned char CPU_HAS_SSE4_1( );
extern unsigned char CPU_HAS_SSE3( );

typedef unsigned int (*T_SAD_FUNCTION)( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
typedef unsigned int (*T_SAD_FUNCTION_CACHED)( unsigned char *address1, unsigned char *address2 );

extern unsigned int SATD_4x4_Intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern unsigned int SATD4x4_INTEL_Intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

extern "C" unsigned int sad_16x16_sse2_ASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern unsigned int SAD_16x16_intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

extern unsigned int SATD_16x16_INTRA_intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern unsigned int SATD_16x16_INTRA_C( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

#ifndef _WIN64
	#ifdef _DEBUG
		#pragma comment (lib, "libs/sad_lib_D_x32.lib")
	#else
		#pragma comment (lib, "libs/sad_lib_R_x32.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment (lib, "libs/sad_lib_D_x64.lib")
	#else
		#pragma comment (lib, "libs/sad_lib_R_x64.lib")
	#endif
#endif

T_SAD_FUNCTION functions[2];
unsigned int (*oldfunc)(unsigned char *, unsigned int, unsigned char *, unsigned int);
unsigned int (*newfunc)(unsigned char *, unsigned int, unsigned char *, unsigned int);

UINT SATD_8x16_intr( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	const unsigned char *pSrc1;
	int src1Step;
	const unsigned char *pSrc2;
	int src2Step;

	pSrc1 = address1;
	pSrc2 = address2;
	src1Step = stride1;
	src2Step = stride2;

	int y;
    UINT satd;
	__declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
	const unsigned char *pS1, *pS2;
    __declspec(align(16)) __m128i  _r_0, _r_1, _r_2, _r_3, _r_5, _r_7, _p_satd;
    static __declspec(align(16)) short _p_one[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    _p_zero = _mm_setzero_si128();
    _p_satd = _mm_setzero_si128();
    pS1 = pSrc1;
    pS2 = pSrc2;
    for( y = 0; y < 16; y += 4 ) {
        _p_0 = _mm_loadl_epi64((__m128i*)(pS1));
        _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
        _p_1 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
        _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
        _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
        _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
        _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
        _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
        _p_0 = _mm_sub_epi16(_p_0, _p_4);
        _p_1 = _mm_sub_epi16(_p_1, _p_5);
        pS1 += 2 * src1Step;
        pS2 += 2 * src2Step;
        _p_2 = _mm_loadl_epi64((__m128i*)(pS1));
        _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
        _p_3 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
        _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
        _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
        _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
        _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
        _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
        _p_2 = _mm_sub_epi16(_p_2, _p_4);
        _p_3 = _mm_sub_epi16(_p_3, _p_5);
        _p_5 = _mm_subs_epi16(_p_0, _p_1);
        _p_0 = _mm_adds_epi16(_p_0, _p_1);
        _p_7 = _mm_subs_epi16(_p_2, _p_3);
        _p_2 = _mm_adds_epi16(_p_2, _p_3);
        _p_1 = _mm_subs_epi16(_p_0, _p_2);
        _p_0 = _mm_adds_epi16(_p_0, _p_2);
        _p_3 = _mm_adds_epi16(_p_5, _p_7);
        _p_5 = _mm_subs_epi16(_p_5, _p_7);

        _r_0 = _mm_unpackhi_epi16(_p_0, _p_1);
        _r_5 = _mm_unpackhi_epi16(_p_5, _p_3);
        pS1 += 2 * src1Step;
        pS2 += 2 * src2Step;

        _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
        _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
        _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
        _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
        _p_1 = _mm_srli_si128(_p_0, 8);
        _p_3 = _mm_srli_si128(_p_7, 8);
        _p_5 = _mm_subs_epi16(_p_0, _p_1);
        _p_0 = _mm_adds_epi16(_p_0, _p_1);
        _p_2 = _mm_subs_epi16(_p_7, _p_3);
        _p_7 = _mm_adds_epi16(_p_7, _p_3);
        _p_1 = _mm_subs_epi16(_p_0, _p_7);
        _p_0 = _mm_adds_epi16(_p_0, _p_7);
        _p_3 = _mm_adds_epi16(_p_5, _p_2);
        _p_5 = _mm_subs_epi16(_p_5, _p_2);
        _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
        _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
        _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
        _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
        _p_3 = _mm_srai_epi16(_p_2, 15);
        _p_1 = _mm_srai_epi16(_p_0, 15);
        _p_2 = _mm_xor_si128(_p_2, _p_3);
        _p_0 = _mm_xor_si128(_p_0, _p_1);
        _p_2 = _mm_sub_epi16(_p_2, _p_3);
        _p_0 = _mm_sub_epi16(_p_0, _p_1);
        _p_0 = _mm_add_epi16(_p_0, _p_2);
        _p_0 = _mm_madd_epi16(_p_0, *(__m128i*)_p_one);
        _p_satd = _mm_add_epi32(_p_satd, _p_0);

        _r_7 = _mm_unpackhi_epi32(_r_0, _r_5);
        _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
        _r_1 = _mm_srli_si128(_r_0, 8);
        _r_3 = _mm_srli_si128(_r_7, 8);
        _r_5 = _mm_subs_epi16(_r_0, _r_1);
        _r_0 = _mm_adds_epi16(_r_0, _r_1);
        _r_2 = _mm_subs_epi16(_r_7, _r_3);
        _r_7 = _mm_adds_epi16(_r_7, _r_3);
        _r_1 = _mm_subs_epi16(_r_0, _r_7);
        _r_0 = _mm_adds_epi16(_r_0, _r_7);
        _r_3 = _mm_adds_epi16(_r_5, _r_2);
        _r_5 = _mm_subs_epi16(_r_5, _r_2);
        _r_0 = _mm_unpacklo_epi16(_r_0, _r_1);
        _r_5 = _mm_unpacklo_epi16(_r_5, _r_3);
        _r_2 = _mm_unpackhi_epi32(_r_0, _r_5);
        _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
        _r_3 = _mm_srai_epi16(_r_2, 15);
        _r_1 = _mm_srai_epi16(_r_0, 15);
        _r_2 = _mm_xor_si128(_r_2, _r_3);
        _r_0 = _mm_xor_si128(_r_0, _r_1);
        _r_2 = _mm_sub_epi16(_r_2, _r_3);
        _r_0 = _mm_sub_epi16(_r_0, _r_1);
        _r_0 = _mm_add_epi16(_r_0, _r_2);
        _r_0 = _mm_madd_epi16(_r_0, *(__m128i*)_p_one);
        _p_satd = _mm_add_epi32(_p_satd, _r_0);
    }
    _p_1 = _mm_srli_si128(_p_satd, 8);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    _p_1 = _mm_srli_si128(_p_satd, 4);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    satd = _mm_cvtsi128_si32(_p_satd);

    return (satd>>1);

}

UINT SATD_8x16_yasm( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	UINT ret = satd_4x4_sse3_YASM( address1, stride1, address2, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 4, stride1, address2 + 4, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 4 * stride1, stride1, address2 + 4 * stride2, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 4 * stride1 + 4, stride1, address2 + 4 * stride2 + 4, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 8 * stride1, stride1, address2 + 8 * stride2, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 8 * stride1 + 4, stride1, address2 + 8 * stride2 + 4, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 12 * stride1, stride1, address2 + 12 * stride2, stride2 );
	ret += satd_4x4_sse3_YASM( address1 + 12 * stride1 + 4, stride1, address2 + 12 * stride2 + 4, stride2 );
	return ret;
}

UINT SATD_8x16_yasm2( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	UINT ret = satd_8x8_sse3_YASM( address1, stride1, address2, stride2 );
	ret += satd_8x8_sse3_YASM( address1 + 8*stride1, stride1, address2 + 8*stride2, stride2 );
	return ret;
}

void DoSATDTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

//	UINT t = SATD_4x4_SSE3( address1, stride1, address2, stride2 );
//	UINT t = satd_4x4_sse3( address1, stride1, address2, stride2 );

	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				for( int mvy=0;mvy<16;mvy+=4)
					for( int mvx=0;mvx<16;mvx+=4)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1)  * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
							{
								unsigned int tres = functions[0]( refa, stride1, cura, stride2 );
								if( mvy >=0 && mvy < MB_SIZE && mvx >=0 && mvx < MB_SIZE )
									orio[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] = tres;
							}
						}
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				for( int mvy=0;mvy<16;mvy+=4)
					for( int mvx=0;mvx<16;mvx+=4)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
							{
//								unsigned int tres = SATD4x4_INTEL_ASM( refa, stride1, cura, stride2 );
//								unsigned int tres = SATD4x4_ASM_SSE2( refa, stride1, cura, stride2 );
//								unsigned int tres = SATD4x4_INTEL( refa, stride1, cura, stride2 );							
//								unsigned int tres = SATD4x4_ASM_MMX( refa, stride1, cura, stride2 );
								unsigned int tres = functions[1]( refa, stride1, cura, stride2 );
								if( mvy >=0 && mvy < MB_SIZE && mvx >=0 && mvx < MB_SIZE )
								{
/*
if( orio[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] != tres )
{
	unsigned int tresold = SATD4x4_INTEL( refa, stride1, cura, stride2 );
printf("here is a mismatch");
}*/
									newo[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] = tres;
								}
							}
						}
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("SATD NxN: MS for old algorithm : %u\n",diffo);
	printf("SATD NxN: MS for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("SATD: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 ? "true":"false");
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE* address1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* address2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	UINT* orio = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	UINT* newo = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( orio, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( newo, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE;i++)
	{
		address1[i] = i+1;
		address2[i] = i;
	}

	if( CPU_HAS_SSE4_1() )
	{
//		functions[0] = SATD_16x16_SSE2_Intrinsic;
//		functions[1] = satd_16x16_sse4_YASM;
	}
	else if( CPU_HAS_SSE3() )
	{
		functions[0] = SATD_8x16_intr;
		functions[1] = SATD_8x16_yasm2;
	}
	else
	{
		printf("OMG how did we end up here ? \n");
//		functions[0] = SATD_16x16_SSE2_Intrinsic;
//		functions[1] = SATD_16x16_SSE2_Intrinsic;
	}/**/


	DoSATDTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( address1 );
	free( address2 );
	free( orio );
	free( newo );
	return 0;

}