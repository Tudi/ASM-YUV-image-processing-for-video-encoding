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

extern "C" void read_luma_inter_pred_avg_16x16_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);
extern "C" void read_luma_inter_pred_avg_8x16_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);
extern "C" void read_luma_inter_pred_avg_16x8_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);
extern "C" void read_luma_inter_pred_avg_8x8_SSE3( const BYTE *address1, const BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst);

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

void read_luma_inter_pred_avg_16x16_C( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst )
{
	BYTE *h1=address1;
	BYTE *h2=address2;
	INT i, j;
	for( i = 0; i < 16; i++ )
	{
		for( j = 0; j < 16; j++ )
		{
			dst[j]=(h1[j]+h2[j]+1 )>>1;
		}
		h1+=stride_src;
		h2+=stride_src;
		dst+=stride_dst;
	}
}

void read_luma_inter_pred_avg_16x16_intrinsic( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst )
{
	for(int i = 0; i < 16; i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r6, r7,
			r0_x, r1_x, r2_x, r3_x, r4_x, r5_x, r6_x, r7_x;
		int stride2 = (stride_src<<1);
		int stride4 = (stride_src<<2);
		int dst_stride2 = (stride_dst<<1);
		int dst_stride4 = (stride_dst<<2);
		r0 = _mm_loadu_si128((__m128i*)(address1));
		r1 = _mm_loadu_si128((__m128i*)(address1+stride_dst));
		r2 = _mm_loadu_si128((__m128i*)(address1+stride2));
		r3 = _mm_loadu_si128((__m128i*)(address1+stride2+stride_src));
		r4 = _mm_loadu_si128((__m128i*)(address1+stride4));
		r5 = _mm_loadu_si128((__m128i*)(address1+stride4+stride_src));
		r6 = _mm_loadu_si128((__m128i*)(address1+stride4+stride2));
		r7 = _mm_loadu_si128((__m128i*)(address1+stride4+stride2+stride_src));
		r0_x = _mm_loadu_si128((__m128i*)(address2));
		r1_x = _mm_loadu_si128((__m128i*)(address2+stride_src));
		r2_x = _mm_loadu_si128((__m128i*)(address2+stride2));
		r3_x = _mm_loadu_si128((__m128i*)(address2+stride2+stride_src));
		r4_x = _mm_loadu_si128((__m128i*)(address2+stride4));
		r5_x = _mm_loadu_si128((__m128i*)(address2+stride4+stride_dst));
		r6_x = _mm_loadu_si128((__m128i*)(address2+stride4+stride2));
		r7_x = _mm_loadu_si128((__m128i*)(address2+stride4+stride2+stride_dst));
		r0 = _mm_avg_epu8(r0, r0_x);
		r1 = _mm_avg_epu8(r1, r1_x);
		r2 = _mm_avg_epu8(r2, r2_x);
		r3 = _mm_avg_epu8(r3, r3_x);
		r4 = _mm_avg_epu8(r4, r4_x);
		r5 = _mm_avg_epu8(r5, r5_x);
		r6 = _mm_avg_epu8(r6, r6_x);
		r7 = _mm_avg_epu8(r7, r7_x);
		_mm_storeu_si128((__m128i*)(dst), r0);
		_mm_storeu_si128((__m128i*)(dst+stride_dst), r1);
		_mm_storeu_si128((__m128i*)(dst+dst_stride2), r2);
		_mm_storeu_si128((__m128i*)(dst+dst_stride2+stride_dst), r3);
		_mm_storeu_si128((__m128i*)(dst+dst_stride4), r4);
		_mm_storeu_si128((__m128i*)(dst+dst_stride4+stride_dst), r5);
		_mm_storeu_si128((__m128i*)(dst+dst_stride4+dst_stride2), r6);
		_mm_storeu_si128((__m128i*)(dst+dst_stride4+dst_stride2+stride_dst), r7);
		address1 += (stride4<<1);
		address2 += (stride4<<1);
		dst += (dst_stride4<<1);
	}
}

void read_luma_inter_pred_avg_8x16_intrinsic( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst )
{
	int i;
	int src_stride = stride_src;
	int dst_stride = stride_dst;
	const unsigned char* src1 = address1;
	const unsigned char* src2 = address2;

	for( i = 0; i < 16; i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r6, r7,
			r0_x, r1_x, r2_x, r3_x, r4_x, r5_x, r6_x, r7_x;
		int stride2 = (src_stride<<1);
		int stride4 = (src_stride<<2);
		int dst_stride2 = (dst_stride<<1);
		int dst_stride4 = (dst_stride<<2);
		r0 = _mm_loadl_epi64((__m128i*)(src1));
		r1 = _mm_loadl_epi64((__m128i*)(src1+src_stride));
		r2 = _mm_loadl_epi64((__m128i*)(src1+stride2));
		r3 = _mm_loadl_epi64((__m128i*)(src1+stride2+src_stride));
		r4 = _mm_loadl_epi64((__m128i*)(src1+stride4));
		r5 = _mm_loadl_epi64((__m128i*)(src1+stride4+src_stride));
		r6 = _mm_loadl_epi64((__m128i*)(src1+stride4+stride2));
		r7 = _mm_loadl_epi64((__m128i*)(src1+stride4+stride2+src_stride));
		r0_x = _mm_loadl_epi64((__m128i*)(src2));
		r1_x = _mm_loadl_epi64((__m128i*)(src2+src_stride));
		r2_x = _mm_loadl_epi64((__m128i*)(src2+stride2));
		r3_x = _mm_loadl_epi64((__m128i*)(src2+stride2+src_stride));
		r4_x = _mm_loadl_epi64((__m128i*)(src2+stride4));
		r5_x = _mm_loadl_epi64((__m128i*)(src2+stride4+src_stride));
		r6_x = _mm_loadl_epi64((__m128i*)(src2+stride4+stride2));
		r7_x = _mm_loadl_epi64((__m128i*)(src2+stride4+stride2+src_stride));
		r0 = _mm_avg_epu8(r0, r0_x);
		r1 = _mm_avg_epu8(r1, r1_x);
		r2 = _mm_avg_epu8(r2, r2_x);
		r3 = _mm_avg_epu8(r3, r3_x);
		r4 = _mm_avg_epu8(r4, r4_x);
		r5 = _mm_avg_epu8(r5, r5_x);
		r6 = _mm_avg_epu8(r6, r6_x);
		r7 = _mm_avg_epu8(r7, r7_x);
		_mm_storel_epi64((__m128i*)(dst), r0);
		_mm_storel_epi64((__m128i*)(dst+dst_stride), r1);
		_mm_storel_epi64((__m128i*)(dst+dst_stride2), r2);
		_mm_storel_epi64((__m128i*)(dst+dst_stride2+dst_stride), r3);
		_mm_storel_epi64((__m128i*)(dst+dst_stride4), r4);
		_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride), r5);
		_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride2), r6);
		_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride2+dst_stride), r7);
		src1 += (stride4<<1);
		src2 += (stride4<<1);
		dst += (dst_stride4<<1);
	}
}

void read_luma_inter_pred_avg_16x8_intrinsic( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst )
{
	int src_stride = stride_src;
	int dst_stride = stride_dst;
	const unsigned char* src1 = address1;
	const unsigned char* src2 = address2;

	__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r6, r7,
		r0_x, r1_x, r2_x, r3_x, r4_x, r5_x, r6_x, r7_x;
	int stride2 = (src_stride<<1);
	int stride4 = (src_stride<<2);
	int dst_stride2 = (dst_stride<<1);
	int dst_stride4 = (dst_stride<<2);
	r0 = _mm_loadu_si128((__m128i*)(src1));
	r1 = _mm_loadu_si128((__m128i*)(src1+src_stride));
	r2 = _mm_loadu_si128((__m128i*)(src1+stride2));
	r3 = _mm_loadu_si128((__m128i*)(src1+stride2+src_stride));
	r4 = _mm_loadu_si128((__m128i*)(src1+stride4));
	r5 = _mm_loadu_si128((__m128i*)(src1+stride4+src_stride));
	r6 = _mm_loadu_si128((__m128i*)(src1+stride4+stride2));
	r7 = _mm_loadu_si128((__m128i*)(src1+stride4+stride2+src_stride));
	r0_x = _mm_loadu_si128((__m128i*)(src2));
	r1_x = _mm_loadu_si128((__m128i*)(src2+src_stride));
	r2_x = _mm_loadu_si128((__m128i*)(src2+stride2));
	r3_x = _mm_loadu_si128((__m128i*)(src2+stride2+src_stride));
	r4_x = _mm_loadu_si128((__m128i*)(src2+stride4));
	r5_x = _mm_loadu_si128((__m128i*)(src2+stride4+src_stride));
	r6_x = _mm_loadu_si128((__m128i*)(src2+stride4+stride2));
	r7_x = _mm_loadu_si128((__m128i*)(src2+stride4+stride2+src_stride));
	r0 = _mm_avg_epu8(r0, r0_x);
	r1 = _mm_avg_epu8(r1, r1_x);
	r2 = _mm_avg_epu8(r2, r2_x);
	r3 = _mm_avg_epu8(r3, r3_x);
	r4 = _mm_avg_epu8(r4, r4_x);
	r5 = _mm_avg_epu8(r5, r5_x);
	r6 = _mm_avg_epu8(r6, r6_x);
	r7 = _mm_avg_epu8(r7, r7_x);
	_mm_storeu_si128((__m128i*)(dst), r0);
	_mm_storeu_si128((__m128i*)(dst+dst_stride), r1);
	_mm_storeu_si128((__m128i*)(dst+dst_stride2), r2);
	_mm_storeu_si128((__m128i*)(dst+dst_stride2+dst_stride), r3);
	_mm_storeu_si128((__m128i*)(dst+dst_stride4), r4);
	_mm_storeu_si128((__m128i*)(dst+dst_stride4+dst_stride), r5);
	_mm_storeu_si128((__m128i*)(dst+dst_stride4+dst_stride2), r6);
	_mm_storeu_si128((__m128i*)(dst+dst_stride4+dst_stride2+dst_stride), r7);
	src1 += (stride4<<1);
	src2 += (stride4<<1);
	dst += (dst_stride4<<1);
}

void read_luma_inter_pred_avg_8x8_intrinsic( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst )
{
	int src_stride = stride_src;
	int dst_stride = stride_dst;
	const unsigned char* src1 = address1;
	const unsigned char* src2 = address2;

	__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r6, r7,
		r0_x, r1_x, r2_x, r3_x, r4_x, r5_x, r6_x, r7_x;
	int stride2 = (src_stride<<1);
	int stride4 = (src_stride<<2);
	int dst_stride2 = (dst_stride<<1);
	int dst_stride4 = (dst_stride<<2);
	r0 = _mm_loadl_epi64((__m128i*)(src1));
	r1 = _mm_loadl_epi64((__m128i*)(src1+src_stride));
	r2 = _mm_loadl_epi64((__m128i*)(src1+stride2));
	r3 = _mm_loadl_epi64((__m128i*)(src1+stride2+src_stride));
	r4 = _mm_loadl_epi64((__m128i*)(src1+stride4));
	r5 = _mm_loadl_epi64((__m128i*)(src1+stride4+src_stride));
	r6 = _mm_loadl_epi64((__m128i*)(src1+stride4+stride2));
	r7 = _mm_loadl_epi64((__m128i*)(src1+stride4+stride2+src_stride));
	r0_x = _mm_loadl_epi64((__m128i*)(src2));
	r1_x = _mm_loadl_epi64((__m128i*)(src2+src_stride));
	r2_x = _mm_loadl_epi64((__m128i*)(src2+stride2));
	r3_x = _mm_loadl_epi64((__m128i*)(src2+stride2+src_stride));
	r4_x = _mm_loadl_epi64((__m128i*)(src2+stride4));
	r5_x = _mm_loadl_epi64((__m128i*)(src2+stride4+src_stride));
	r6_x = _mm_loadl_epi64((__m128i*)(src2+stride4+stride2));
	r7_x = _mm_loadl_epi64((__m128i*)(src2+stride4+stride2+src_stride));
	r0 = _mm_avg_epu8(r0, r0_x);
	r1 = _mm_avg_epu8(r1, r1_x);
	r2 = _mm_avg_epu8(r2, r2_x);
	r3 = _mm_avg_epu8(r3, r3_x);
	r4 = _mm_avg_epu8(r4, r4_x);
	r5 = _mm_avg_epu8(r5, r5_x);
	r6 = _mm_avg_epu8(r6, r6_x);
	r7 = _mm_avg_epu8(r7, r7_x);
	_mm_storel_epi64((__m128i*)(dst), r0);
	_mm_storel_epi64((__m128i*)(dst+dst_stride), r1);
	_mm_storel_epi64((__m128i*)(dst+dst_stride2), r2);
	_mm_storel_epi64((__m128i*)(dst+dst_stride2+dst_stride), r3);
	_mm_storel_epi64((__m128i*)(dst+dst_stride4), r4);
	_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride), r5);
	_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride2), r6);
	_mm_storel_epi64((__m128i*)(dst+dst_stride4+dst_stride2+dst_stride), r7);
	src1 += (stride4<<1);
	src2 += (stride4<<1);
	dst += (dst_stride4<<1);
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
//					read_luma_inter_pred_avg_16x16_C( refa, cura, stride1, (BYTE*)out, stride2 );
//					read_luma_inter_pred_avg_16x16_intrinsic( refa, cura, stride1, (BYTE*)out, stride2 );
//					read_luma_inter_pred_avg_8x16_intrinsic( refa, cura, stride1, (BYTE*)out, stride2 );
//					read_luma_inter_pred_avg_16x8_intrinsic( refa, cura, stride1, (BYTE*)out, stride2 );
					read_luma_inter_pred_avg_8x8_intrinsic( refa, cura, stride1, (BYTE*)out, stride2 );
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
//					read_luma_inter_pred_avg_16x16_SSE3( refa, cura, stride1, (BYTE*)out, stride2 );
//					read_luma_inter_pred_avg_8x16_SSE3( refa, cura, stride1, (BYTE*)out, stride2 );
//					read_luma_inter_pred_avg_16x8_SSE3( refa, cura, stride1, (BYTE*)out, stride2 );
					read_luma_inter_pred_avg_8x8_SSE3( refa, cura, stride1, (BYTE*)out, stride2 );
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