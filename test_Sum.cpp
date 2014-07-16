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

extern "C" int sum_16x16_sse3_YASM( unsigned char *address1, unsigned int stride1 );

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

int sum_16x16( unsigned char *address1, unsigned int stride1 )
{
	int SumS[16];
	for( int i=0;i<16;i++)
	{
		SumS[i]=0;
		for(int j=0;j<16;j++)
			SumS[i] += address1[i*stride1+j];
	}
	int ret = 0;
	for( int i=0;i<16;i++)
		ret += SumS[i];
	return ret;
}

int sum_8x8( unsigned char *address1, unsigned int stride1 )
{
	int ret = 0;
	for( int i=0;i<8;i++)
	{
		int localret = 0;
		for(int j=0;j<8;j++)
			localret += address1[j];
		ret += localret;
		address1 += stride1;
	}
	return ret;
}
int sum_16x16_2( unsigned char *address1, unsigned int stride1 )
{
	int ret;

	ret = sum_8x8( address1, stride1 );
	ret += sum_8x8( address1+8, stride1 );
	ret += sum_8x8( address1+8*stride1, stride1 );
	ret += sum_8x8( address1+8*stride1+8, stride1 );
	return ret;
}

int sum_4x4( unsigned char *address1, unsigned int stride1 )
{
	int ret = 0;
	for( int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
			ret += address1[j];
		address1 += stride1;
	}
	return ret;
}
int sum_8x8_2( unsigned char *address1, unsigned int stride1 )
{
	int ret;
	ret = sum_4x4( address1, stride1 );
	ret += sum_4x4( address1+4, stride1 );
	ret += sum_4x4( address1+4*stride1, stride1 );
	ret += sum_4x4( address1+4*stride1+4, stride1 );
	return ret;
}
int sum_16x16_3( unsigned char *address1, unsigned int stride1 )
{
	int ret;

	ret = sum_8x8_2( address1, stride1 );
	ret += sum_8x8_2( address1+4*stride1, stride1 );
	ret += sum_8x8_2( address1+4, stride1 );
	ret += sum_8x8_2( address1+4*stride1+4, stride1 );
	return ret;
}
int sum_16x16_4( unsigned char *address1, unsigned int stride1 )
{
	int SumS[16];
	memset( SumS, 0, sizeof( SumS ) );

	#pragma omp parallel for 
	for(int i=0;i<16;i++)
	{
		unsigned char *address = address1 + stride1 * i;
		int *sum = &SumS[i];
		for(int j=0;j<16;j++)
			*sum += address[j];
	}

	int ret=0;
	for( int i=0;i<16;i++)
		ret += SumS[i];
	return ret;
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
					*out = sum_16x16_2( refa, stride1 );
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
					*out = sum_16x16_sse3_YASM( refa, stride1 );
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

	DoSpeedTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( address1 );
	free( address2 );
	free( orio );
	free( newo );
	return 0;

}