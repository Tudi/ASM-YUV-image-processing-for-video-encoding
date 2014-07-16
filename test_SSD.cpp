#include "StdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>

#define MB_X_REF	68
#define MB_Y_REF	45
#define MB_SIZE		16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
	#define LOOP_TEST_COUNT			1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT_INTERP	50	//to have a larger runtime
	#define LOOP_TEST_COUNT			100	//to have a larger runtime
#endif

extern "C" int ssd_16x16_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" int sad_16x16_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" int sad_16x16_sse3_YASM2( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

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

UINT SSD_16x16(BYTE *address1, INT stride1, BYTE *address2, INT stride2)
{
	
	int i;
	__declspec(align(16)) __m128i ssd,zero,s0,s1,r0,max_v,min_v;
	ssd = _mm_setzero_si128();
	zero = _mm_setzero_si128();
	int ssd_result;
	for( i = 0; i < 16; i++ )
	{
		s0 = _mm_loadu_si128((__m128i*)address1);
		s1 = _mm_loadu_si128((__m128i*)address2);

		max_v = _mm_max_epu8(s0,s1);
		min_v = _mm_min_epu8(s0,s1);

		r0 = _mm_sub_epi8(max_v,min_v);			//Mc : can further optimize?

		s0 = _mm_unpacklo_epi8(r0, zero);
		s1 = _mm_unpackhi_epi8(r0, zero);

		s0 = _mm_madd_epi16(s0,s0);
		s1 = _mm_madd_epi16(s1,s1);

		ssd = _mm_add_epi32(ssd,s0);
		ssd = _mm_add_epi32(ssd,s1);
		address1+=stride1;
		address2+=stride2;

	}

	r0 = _mm_srli_si128(ssd,8);
	ssd = _mm_add_epi32(r0,ssd);
	r0 = _mm_srli_si128(ssd,4);
	ssd = _mm_add_epi32(r0,ssd);

	ssd_result = _mm_cvtsi128_si32(ssd);

	return ssd_result;
}

UINT SAD_16x16( BYTE *address1, INT stride1, BYTE *address2, INT stride2 )
{
	UINT sad;
	unsigned short sad_array[8];
	__m128i l0, l1, line_sad, acc_sad;

	acc_sad = _mm_setzero_si128();
	//acc_sad = _mm_xor_si128(acc_sad, acc_sad);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	_mm_storeu_si128((__m128i*)(&sad_array[0]), acc_sad);

	return (sad_array[0]+sad_array[4]);
}

void DoSpeedTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				int index = ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
				BYTE* refa = address1 + index;
				BYTE* cura = address2 + index;
				BYTE *out =  (BYTE*)orio + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					*out = SSD_16x16( refa, stride1, cura, stride2 );
//					*out = SAD_16x16( refa, stride1, cura, stride2 );
//					*out = sad_16x16_sse3_YASM( refa, stride1, cura, stride2 );
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				int index = ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
				BYTE* refa = address1 + index;
				BYTE* cura = address2 + index;
				BYTE *out =  (BYTE*)newo + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					*out = ssd_16x16_sse3_YASM( refa, stride1, cura, stride2 );
//					*out = sad_16x16_sse3_YASM2( refa, stride1, cura, stride2 );
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("MS for old algorithm : %u\n",diffo);
	printf("for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 ? "true":"false");
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE *taddress1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) + 32);
	BYTE *taddress2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) + 32 );
	UINT* torio = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
	UINT* tnewo = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
	BYTE* address1;
	BYTE* address2;
	UINT* orio;
	UINT* newo;

	//break alignment to get worst case scenario
	address1 = UnAllignAddress( taddress1 );
	address2 = UnAllignAddress( taddress2 );
	orio = UnAllignAddress( torio );
	newo = UnAllignAddress( tnewo );

	memset( orio, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( newo, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE;i++)
	{
		address1[i] = i+1;
		address2[i] = i;
	}

	DoSpeedTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( taddress1 );
	free( taddress2 );
	free( torio );
	free( tnewo );

	return 0;

}