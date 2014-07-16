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
	#define LOOP_TEST_COUNT_LARGE	15	//to have a larger runtime
	#define LOOP_TEST_COUNT			10	//to have a larger runtime
#endif

extern "C" unsigned int sum_16x16_sse3_YASM( unsigned char *address, unsigned int stride);
extern unsigned char CPU_HAS_SSSE3( );

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

inline void cal_mean_variance_HalfIntrinsic(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i s0, s1, s2, s3, s4, s5, s6, s7; 
	__m128i m0;
	__m128i sum, zero;
	unsigned int mean=0;
	unsigned int var=0;
	unsigned int mean1=0;
	unsigned int var1=0;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
	if( CPU_HAS_SSSE3() )
	{
		for(int y = 0; y < (height); y+=16)
		{
			for(int x = 0; x < (width); x+=16)
			{
				pSrc = &src_ptr[x];
				
				mean += sum_16x16_sse3_YASM(pSrc,stride);
			}
			src_ptr += (stride<<4);
		}
	}

	mean=(mean/(width*height));
	*result_m=mean;

	src_ptr=src_buf;
	m0 = _mm_set1_epi16((short)mean);
	sum = _mm_setzero_si128();
	var=0;
	for(int y = 0; y < height; y+=4)
	{
		for(int x = 0; x < width; x+=16)
		{
			pSrc = &src_ptr[x];
			s0 = _mm_loadu_si128((__m128i*)pSrc);	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s0, s0));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s1, s1));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s2, s2));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s3, s3));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s4, s4));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s5, s5));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s6, s6));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s7, s7));

		}
		src_ptr += (stride<<2);
	}
	var += (sum.m128i_i32[0]);
	var += (sum.m128i_i32[1]);
	var += (sum.m128i_i32[2]);
	var += (sum.m128i_i32[3]);

	var=(var/(width*height));
	*result_v=var;
}

inline void cal_mean_variance_Intrinsic(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i s0, s1, s2, s3, s4, s5, s6, s7; 
	__m128i m0;
	__m128i sum, zero;
	unsigned int mean=0;
	unsigned int var=0;
	unsigned int mean1=0;
	unsigned int var1=0;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
	{
		for(int y = 0; y < (height); y+=16)
		{
			for(int x = 0; x < (width); x+=16)
			{
				pSrc = &src_ptr[x];
				
				s0 = _mm_loadu_si128((__m128i*)pSrc);
				s1 = _mm_loadu_si128((__m128i*)(pSrc+stride));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+4*stride));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+5*stride));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+6*stride));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+7*stride));			
				sum = _mm_sad_epu8(s0,zero); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s1,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s2,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s3,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s4,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s5,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s6,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s7,zero)); 

				s0 = _mm_loadu_si128((__m128i*)(pSrc+8*stride));
				s1 = _mm_loadu_si128((__m128i*)(pSrc+9*stride));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+10*stride));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+11*stride));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+12*stride));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+13*stride));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+14*stride));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+15*stride));

				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s0,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s1,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s2,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s3,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s4,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s5,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s6,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s7,zero)); 

				mean += _mm_extract_epi16(sum,0) + _mm_extract_epi16(sum,4);

			}
			src_ptr += (stride<<4);
		}
	}
	mean=(mean/(width*height));
	*result_m=mean;

	src_ptr=src_buf;
	m0 = _mm_set1_epi16((short)mean);
	sum = _mm_setzero_si128();
	var=0;
	for(int y = 0; y < height; y+=4)
	{
		for(int x = 0; x < width; x+=16)
		{
			pSrc = &src_ptr[x];
			s0 = _mm_loadu_si128((__m128i*)pSrc);	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s0, s0));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s1, s1));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s2, s2));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s3, s3));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s4, s4));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s5, s5));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s6, s6));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s7, s7));

		}
		src_ptr += (stride<<2);
	}
	var += (sum.m128i_i32[0]);
	var += (sum.m128i_i32[1]);
	var += (sum.m128i_i32[2]);
	var += (sum.m128i_i32[3]);

	var=(var/(width*height));
	*result_v=var;
}

inline void cal_mean_variance_Intrinsic2(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i s0, s1, s2, s3, s4, s5, s6, s7; 
	__m128i m0;
	__m128i sum, zero;
	unsigned int mean=0;
	unsigned int var=0;
	unsigned int mean1=0;
	unsigned int var1=0;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
	{
		for(int y = 0; y < (height); y+=16)
		{
			for(int x = 0; x < (width); x+=16)
			{
				pSrc = &src_ptr[x];
				
				s0 = _mm_loadu_si128((__m128i*)pSrc);
				s1 = _mm_loadu_si128((__m128i*)(pSrc+stride));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+4*stride));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+5*stride));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+6*stride));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+7*stride));			

				s0 = _mm_sad_epu8(s0,zero);
				s1 = _mm_sad_epu8(s1,zero);
				s2 = _mm_sad_epu8(s2,zero);
				s3 = _mm_sad_epu8(s3,zero);
				s4 = _mm_sad_epu8(s4,zero);
				s5 = _mm_sad_epu8(s5,zero);
				s6 = _mm_sad_epu8(s6,zero);
				s7 = _mm_sad_epu8(s7,zero);

				s0 = _mm_adds_epu16(s0 ,s1 ); 
				s2 = _mm_adds_epu16(s2 ,s3 ); 
				s4 = _mm_adds_epu16(s4 ,s5 ); 
				s6 = _mm_adds_epu16(s6 ,s7 ); 
				s0 = _mm_adds_epu16(s0 ,s2 ); 
				s4 = _mm_adds_epu16(s4 ,s6 ); 
				sum = _mm_adds_epu16(s0, s4); 

				s0 = _mm_loadu_si128((__m128i*)(pSrc+8*stride));
				s1 = _mm_loadu_si128((__m128i*)(pSrc+9*stride));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+10*stride));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+11*stride));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+12*stride));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+13*stride));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+14*stride));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+15*stride));

				s0 = _mm_sad_epu8(s0,zero);
				s1 = _mm_sad_epu8(s1,zero);
				s2 = _mm_sad_epu8(s2,zero);
				s3 = _mm_sad_epu8(s3,zero);
				s4 = _mm_sad_epu8(s4,zero);
				s5 = _mm_sad_epu8(s5,zero);
				s6 = _mm_sad_epu8(s6,zero);
				s7 = _mm_sad_epu8(s7,zero);

				s0 = _mm_adds_epu16(s0 ,s1 ); 
				s2 = _mm_adds_epu16(s2 ,s3 ); 
				s4 = _mm_adds_epu16(s4 ,s5 ); 
				s6 = _mm_adds_epu16(s6 ,s7 ); 
				s0 = _mm_adds_epu16(s0 ,s2 ); 
				s4 = _mm_adds_epu16(s4 ,s6 ); 
				sum = _mm_adds_epu16(sum, s0); 
				sum = _mm_adds_epu16(sum, s4); 

				mean += _mm_extract_epi16(sum,0) + _mm_extract_epi16(sum,4);

			}
			src_ptr += (stride<<4);
		}
	}
	mean=(mean/(width*height));
	*result_m=mean;

	src_ptr=src_buf;
	m0 = _mm_set1_epi16((short)mean);
	sum = _mm_setzero_si128();
	var=0;
	for(int y = 0; y < height; y+=16)
	{
		for(int x = 0; x < width; x+=16)
		{
			pSrc = &src_ptr[x];
			s0 = _mm_loadu_si128((__m128i*)(pSrc+0*stride));	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+1*stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			s0 = _mm_madd_epi16 (s0, s0);
			s1 = _mm_madd_epi16 (s1, s1);
			s2 = _mm_madd_epi16 (s2, s2);
			s3 = _mm_madd_epi16 (s3, s3);
			s4 = _mm_madd_epi16 (s4, s4);
			s5 = _mm_madd_epi16 (s5, s5);
			s6 = _mm_madd_epi16 (s6, s6);
			s7 = _mm_madd_epi16 (s7, s7);

			s0 = _mm_add_epi32 (s0, s1);
			s2 = _mm_add_epi32 (s2, s3);
			s4 = _mm_add_epi32 (s4, s5);
			s6 = _mm_add_epi32 (s6, s7);
			s0 = _mm_add_epi32 (s0, s2);
			s4 = _mm_add_epi32 (s4, s6);
			sum = _mm_add_epi32 (sum, s0);
			sum = _mm_add_epi32 (sum, s4);

			s0 = _mm_loadu_si128((__m128i*)(pSrc+4*stride));	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+5*stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+6*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+7*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			s0 = _mm_madd_epi16 (s0, s0);
			s1 = _mm_madd_epi16 (s1, s1);
			s2 = _mm_madd_epi16 (s2, s2);
			s3 = _mm_madd_epi16 (s3, s3);
			s4 = _mm_madd_epi16 (s4, s4);
			s5 = _mm_madd_epi16 (s5, s5);
			s6 = _mm_madd_epi16 (s6, s6);
			s7 = _mm_madd_epi16 (s7, s7);

			s0 = _mm_add_epi32 (s0, s1);
			s2 = _mm_add_epi32 (s2, s3);
			s4 = _mm_add_epi32 (s4, s5);
			s6 = _mm_add_epi32 (s6, s7);
			s0 = _mm_add_epi32 (s0, s2);
			s4 = _mm_add_epi32 (s4, s6);
			sum = _mm_add_epi32 (sum, s0);
			sum = _mm_add_epi32 (sum, s4);

			s0 = _mm_loadu_si128((__m128i*)(pSrc+8*stride));	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+9*stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+10*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+11*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			s0 = _mm_madd_epi16 (s0, s0);
			s1 = _mm_madd_epi16 (s1, s1);
			s2 = _mm_madd_epi16 (s2, s2);
			s3 = _mm_madd_epi16 (s3, s3);
			s4 = _mm_madd_epi16 (s4, s4);
			s5 = _mm_madd_epi16 (s5, s5);
			s6 = _mm_madd_epi16 (s6, s6);
			s7 = _mm_madd_epi16 (s7, s7);

			s0 = _mm_add_epi32 (s0, s1);
			s2 = _mm_add_epi32 (s2, s3);
			s4 = _mm_add_epi32 (s4, s5);
			s6 = _mm_add_epi32 (s6, s7);
			s0 = _mm_add_epi32 (s0, s2);
			s4 = _mm_add_epi32 (s4, s6);
			sum = _mm_add_epi32 (sum, s0);
			sum = _mm_add_epi32 (sum, s4);

			s0 = _mm_loadu_si128((__m128i*)(pSrc+12*stride));	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+13*stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+14*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+15*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			s0 = _mm_madd_epi16 (s0, s0);
			s1 = _mm_madd_epi16 (s1, s1);
			s2 = _mm_madd_epi16 (s2, s2);
			s3 = _mm_madd_epi16 (s3, s3);
			s4 = _mm_madd_epi16 (s4, s4);
			s5 = _mm_madd_epi16 (s5, s5);
			s6 = _mm_madd_epi16 (s6, s6);
			s7 = _mm_madd_epi16 (s7, s7);

			s0 = _mm_add_epi32 (s0, s1);
			s2 = _mm_add_epi32 (s2, s3);
			s4 = _mm_add_epi32 (s4, s5);
			s6 = _mm_add_epi32 (s6, s7);
			s0 = _mm_add_epi32 (s0, s2);
			s4 = _mm_add_epi32 (s4, s6);
			sum = _mm_add_epi32 (sum, s0);
			sum = _mm_add_epi32 (sum, s4);
		}
		src_ptr += (stride*16);
	}
	var += (sum.m128i_i32[0]);
	var += (sum.m128i_i32[1]);
	var += (sum.m128i_i32[2]);
	var += (sum.m128i_i32[3]);

	var=(var/(width*height));
	*result_v=var;
}

//this is made just to watch how bad code optimization can get
inline void cal_mean_variance_Intrinsic3(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i m0;
	__m128i sum, zero;
	unsigned int mean=0;
	unsigned int var=0;
	unsigned int mean1=0;
	unsigned int var1=0;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
	sum = _mm_setzero_si128();
	int Counter=126;
	for(int y = 0; y < (height); y++)
	{
		for(int x = 0; x < (width); x+=16)
		{
			__m128i s0 = _mm_loadu_si128((__m128i*)(&src_ptr[x]));
			s0 = _mm_sad_epu8(s0,zero);
			sum = _mm_adds_epu16(s0 ,sum ); 
			Counter--;
			if( Counter == 0 )
			{
				mean += _mm_extract_epi16(sum,0) + _mm_extract_epi16(sum,4);
				Counter = 126;
				sum = _mm_setzero_si128();
			}
		}
		src_ptr += (stride*1);
	}
	mean += _mm_extract_epi16(sum,0) + _mm_extract_epi16(sum,4);
	mean=(mean/(width*height));
	*result_m=mean;

	src_ptr=src_buf;
	m0 = _mm_set1_epi16((short)mean);
	sum = _mm_setzero_si128();
	var=0;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x+=16)
		{
			__m128i s0,s1;
			s0 = _mm_loadu_si128((__m128i*)(&src_ptr[x]));	//0...7|8...15
			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s0 = _mm_madd_epi16 (s0, s0);
			s1 = _mm_madd_epi16 (s1, s1);
			sum = _mm_add_epi32 (sum, s0);
			sum = _mm_add_epi32 (sum, s1);
		}
		src_ptr += (stride*1);
	}
	var += (sum.m128i_i32[0]);
	var += (sum.m128i_i32[1]);
	var += (sum.m128i_i32[2]);
	var += (sum.m128i_i32[3]);

	var=(var/(width*height));
	*result_v=var;
}

inline unsigned int cal_mean(unsigned char* src_buf, int width, int height, int stride)
{
	unsigned int mean=0;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
			mean+=(src_buf[x]);
		src_buf += stride;
	}
	mean=(mean/(width*height));
	return mean;
}

inline void cal_mean_variance_C(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i s0, s1, s2, s3, s4, s5, s6, s7; 
	__m128i m0;
	__m128i sum, zero;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
	unsigned int var=0;
	unsigned int mean=cal_mean(src_buf, width, height, stride);
	*result_m=mean;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
			var+=((src_buf[x]-mean)*(src_buf[x]-mean));
		src_buf += stride;
	}
	var=(var/(width*height));
	*result_v=var;
}


inline void cal_mean_variance_Intrinsic_Continues(unsigned char* src_buf, int width, int height, int stride, unsigned int *result_m, unsigned int *result_v)
{
	unsigned char* src_ptr;
	unsigned char* pSrc;
	__m128i s0, s1, s2, s3, s4, s5, s6, s7; 
	__m128i m0;
	__m128i sum, zero;
	unsigned int mean=0;
	unsigned int var=0;
	unsigned int mean1=0;
	unsigned int var1=0;

	src_ptr=src_buf;
	zero = _mm_setzero_si128();
//	int AlignmentShift = (16-(int)src_buf&0x0F);
//	unsigned char *AllignedSrc = src_buf + AlignmentShift;
//	if( CPU_HAS_SSSE3() )
	{
		for( int i=0;i<height*stride;i+=16*16 )
		{
				pSrc = &src_buf[i];
				
				s0 = _mm_loadu_si128((__m128i*)(pSrc+0*16));
				s1 = _mm_loadu_si128((__m128i*)(pSrc+1*16));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+2*16));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+3*16));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+4*16));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+5*16));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+6*16));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+7*16));			
				sum = _mm_sad_epu8(s0,zero); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s1,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s2,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s3,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s4,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s5,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s6,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s7,zero)); 

				s0 = _mm_loadu_si128((__m128i*)(pSrc+8*16));
				s1 = _mm_loadu_si128((__m128i*)(pSrc+9*16));
				s2 = _mm_loadu_si128((__m128i*)(pSrc+10*16));
				s3 = _mm_loadu_si128((__m128i*)(pSrc+11*16));
				s4 = _mm_loadu_si128((__m128i*)(pSrc+12*16));
				s5 = _mm_loadu_si128((__m128i*)(pSrc+13*16));
				s6 = _mm_loadu_si128((__m128i*)(pSrc+14*16));
				s7 = _mm_loadu_si128((__m128i*)(pSrc+15*16));

				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s0,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s1,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s2,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s3,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s4,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s5,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s6,zero)); 
				sum = _mm_adds_epu16(sum, _mm_sad_epu8(s7,zero)); 

				mean += _mm_extract_epi16(sum,0) + _mm_extract_epi16(sum,4);
		}
	}

	mean=(mean/(width*height));
	*result_m=mean;

	src_ptr=src_buf;
	m0 = _mm_set1_epi16((short)mean);
	sum = _mm_setzero_si128();
	var=0;
	for(int y = 0; y < height; y+=4)
	{
		for(int x = 0; x < width; x+=16)
		{
			pSrc = &src_ptr[x];
			s0 = _mm_loadu_si128((__m128i*)(pSrc+0*stride));	//0...7|8...15
			s2 = _mm_loadu_si128((__m128i*)(pSrc+1*stride));
			s4 = _mm_loadu_si128((__m128i*)(pSrc+2*stride));
			s6 = _mm_loadu_si128((__m128i*)(pSrc+3*stride));

			s1 = _mm_unpackhi_epi8(s0, zero);		//8...15
			s0 = _mm_unpacklo_epi8(s0, zero);		//0...7
			s3 = _mm_unpackhi_epi8(s2, zero);		//8...15
			s2 = _mm_unpacklo_epi8(s2, zero);		//0...7
			s5 = _mm_unpackhi_epi8(s4, zero);		//8...15
			s4 = _mm_unpacklo_epi8(s4, zero);		//0...7
			s7 = _mm_unpackhi_epi8(s6, zero);		//8...15
			s6 = _mm_unpacklo_epi8(s6, zero);		//0...7

			s0 = _mm_sub_epi16(s0, m0);
			s1 = _mm_sub_epi16(s1, m0);
			s2 = _mm_sub_epi16(s2, m0);
			s3 = _mm_sub_epi16(s3, m0);
			s4 = _mm_sub_epi16(s4, m0);
			s5 = _mm_sub_epi16(s5, m0);
			s6 = _mm_sub_epi16(s6, m0);
			s7 = _mm_sub_epi16(s7, m0);

			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s0, s0));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s1, s1));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s2, s2));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s3, s3));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s4, s4));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s5, s5));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s6, s6));
			sum = _mm_add_epi32 (sum, _mm_madd_epi16 (s7, s7));
		}
		src_ptr += (stride<<2);
	}
	var += (sum.m128i_i32[0]);
	var += (sum.m128i_i32[1]);
	var += (sum.m128i_i32[2]);
	var += (sum.m128i_i32[3]);

	var=(var/(width*height));
	*result_v=var;
}

void DoSpeedTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	for( int loop=0;loop<LOOP_TEST_COUNT_LARGE;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
//			cal_mean_variance_C( address1, MB_X_REF * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, orio, orio + 1 );
			cal_mean_variance_Intrinsic( address1, (MB_X_REF-4) * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, orio, orio + 1 );
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
			cal_mean_variance_HalfIntrinsic( address1, (MB_X_REF-4) * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, newo, newo + 1 );
//			cal_mean_variance_Intrinsic_Continues( address1, (MB_X_REF-4) * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, newo, newo + 1 );
//			cal_mean_variance_Intrinsic2( address1, (MB_X_REF-4) * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, newo, newo + 1 );
//			cal_mean_variance_Intrinsic3( address1, (MB_X_REF-4) * MB_SIZE, MB_Y_REF * MB_SIZE, stride1, newo, newo + 1 );
		en = GetTickCount();
		diffn += en - sn;
	}

	printf("MS for old algorithm : %u\n",diffo);
	printf("for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	int ResultMatches = memcmp( orio,newo, 8 );
	printf("Results match : %s \n", ResultMatches == 0 ? "true":"false");
}

int main()
{
	int stride = MB_X_REF * MB_SIZE;
	int antioptimizer;
	BYTE *taddress1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32);
	BYTE *taddress2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) + 32 );
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
		((short*)address1)[i] = i % 255;
		((short*)address2)[i] = i % 255;
	}
	printf("Please write 1 and press enter:");
	scanf("%d",&antioptimizer);
	stride = stride * antioptimizer;
	DoSpeedTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( taddress1 );
	free( taddress2 );
	free( torio );
	free( tnewo );

	return 0;

}