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

extern "C" void Separate16x16_SSE3_YASM(BYTE * src, INT src_stride, BYTE * pred, INT pred_stride, SHORT * texture, INT texture_stride);

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

void SeparatePrediction16x16_intrinsic( BYTE * src, INT src_stride, BYTE * pred, INT pred_stride, SHORT * texture, INT texture_stride )
{
	__m128i l0, l1, l2, l3, out1, out2, zero;
	zero = _mm_setzero_si128();

	for( int i = 0; i < 16; i += 2)
	{ 
		l0 = _mm_loadu_si128((__m128i*)src);
		l1 = _mm_loadu_si128((__m128i*)pred);
		l2 = _mm_unpackhi_epi8(l0, zero);
		l3 = _mm_unpackhi_epi8(l1, zero);
		l0 = _mm_unpacklo_epi8(l0, zero);
		l1 = _mm_unpacklo_epi8(l1, zero);
		out1 = _mm_sub_epi16(l0, l1);
		out2 = _mm_sub_epi16(l2, l3);
		_mm_storeu_si128((__m128i*)texture, out1);
		_mm_storeu_si128((__m128i*)(texture+8), out2);

		src += src_stride;
		pred += pred_stride;
		texture += texture_stride;

		l0 = _mm_loadu_si128((__m128i*)src);
		l1 = _mm_loadu_si128((__m128i*)pred);
		l2 = _mm_unpackhi_epi8(l0, zero);
		l3 = _mm_unpackhi_epi8(l1, zero);
		l0 = _mm_unpacklo_epi8(l0, zero);
		l1 = _mm_unpacklo_epi8(l1, zero);
		out1 = _mm_sub_epi16(l0, l1);
		out2 = _mm_sub_epi16(l2, l3);
		_mm_storeu_si128((__m128i*)texture, out1);
		_mm_storeu_si128((__m128i*)(texture+8), out2);

		src += src_stride;
		pred += pred_stride;
		texture += texture_stride;
	}
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
				SHORT *out =  (SHORT*)orio + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					SeparatePrediction16x16_intrinsic( refa, stride1, cura, stride2, out, stride1 );
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
				SHORT *out =  (SHORT*)newo + index;
				for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
					Separate16x16_SSE3_YASM( refa, stride1, cura, stride2, out, stride1 );
;
			}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("MS for old algorithm : %u\n",diffo);
	printf("for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) * 4 ) == 0 ? "true":"false");
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE *taddress1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) + 32);
	BYTE *taddress2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) + 32 );
	UINT* torio = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) * 4 + 32 );
	UINT* tnewo = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) * 4 + 32 );
	BYTE* address1;
	BYTE* address2;
	UINT* orio;
	UINT* newo;

	//break alignment to get worst case scenario
	address1 = UnAllignAddress( taddress1 );
	address2 = UnAllignAddress( taddress2 );
	orio = UnAllignAddress( torio );
	newo = UnAllignAddress( tnewo );

	memset( orio, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) * 4 );
	memset( newo, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) * 4 );
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